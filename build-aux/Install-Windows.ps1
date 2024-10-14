[CmdletBinding()]
param(
    [ValidateSet('x64')]
    [string] $Target = 'x64',
    [ValidateSet('Debug', 'RelWithDebInfo', 'Release', 'MinSizeRel')]
    [string] $Configuration = 'RelWithDebInfo',
    [ValidateSet('Installer', 'Archive', 'Testing')]
    [string] $Intent = 'Testing'
)

$ErrorActionPreference = 'Stop'

if ( $DebugPreference -eq 'Continue' ) {
    $VerbosePreference = 'Continue'
    $InformationPreference = 'Continue'
}

if ( ! ( [System.Environment]::Is64BitOperatingSystem ) ) {
    throw "A 64-bit system is required to build the project."
}

Write-Debug $PSVersionTable.PSVersion
if ( $PSVersionTable.PSVersion -lt '7.0.0' ) {
    Write-Warning 'The obs-deps PowerShell build script requires PowerShell Core 7. Install or upgrade your PowerShell version: https://aka.ms/pscore6'
    exit 2
}

function Log-Debug {
    [CmdletBinding()]
    param(
        [Parameter(Mandatory,ValueFromPipeline)]
        [ValidateNotNullOrEmpty()]
        [string[]] $Message
    )

    Process {
        foreach($m in $Message) {
            Write-Debug "$(if ( $env:CI -ne $null ) { '::debug::' })$m"
        }
    }
}

function Log-Error {
    [CmdletBinding()]
    param(
        [Parameter(Mandatory,ValueFromPipeline)]
        [ValidateNotNullOrEmpty()]
        [string[]] $Message
    )

    Process {
        foreach($m in $Message) {
            Write-Error "$(if ( $env:CI -ne $null ) { '::error::' })$m"
        }
    }
}

function Log-Information {
    [CmdletBinding()]
    param(
        [Parameter(Mandatory,ValueFromPipeline)]
        [ValidateNotNullOrEmpty()]
        [string[]] $Message
    )

    Process {
        if ( ! ( $script:Quiet ) ) {
            $StageName = $( if ( $script:StageName -ne $null ) { $script:StageName } else { '' })
            $Icon = ' =>'

            foreach($m in $Message) {
                Write-Host -NoNewLine -ForegroundColor Blue "  ${StageName} $($Icon.PadRight(5)) "
                Write-Host "${m}"
            }
        }
    }
}

function Log-Group {
    [CmdletBinding()]
    param(
        [Parameter(ValueFromPipeline=$true)]
        [string[]] $Message,

        [Parameter()]
        [switch] $Silent
    )

    Process {

        if ($Silent -and -not $Debug) {
            # Suppress output
            return
        }

        if ($Message.Count -ge 1) {
            if ($PSCmdlet.MyInvocation.BoundParameters.ContainsKey('Debug')) 
            {
                Log-Debug -Message $Message
            } else {
                Log-Information -Message $Message
            }
        }
    }
}

function Ensure-Location {
    <#
        .SYNOPSIS
            Ensures current location to be set to specified directory.
        .DESCRIPTION
            If specified directory exists, switch to it. Otherwise create it,
            then switch.
        .EXAMPLE
            Ensure-Location "My-Directory"
            Ensure-Location -Path "Path-To-My-Directory"
    #>

    param(
        [Parameter(Mandatory)]
        [string] $Path
    )

    if ( ! ( Test-Path $Path ) ) {
        $_Params = @{
            ItemType = "Directory"
            Path = ${Path}
            ErrorAction = "SilentlyContinue"
        }

        New-Item @_Params | Set-Location
    } else {
        Set-Location -Path ${Path}
    }
}

function Invoke-External {
    <#
        .SYNOPSIS
            Invokes a non-PowerShell command.
        .DESCRIPTION
            Runs a non-PowerShell command, and captures its return code.
            Throws an exception if the command returns non-zero.
        .EXAMPLE
            Invoke-External 7z x $MyArchive
    #>

    if ( $args.Count -eq 0 ) {
        throw 'Invoke-External called without arguments.'
    }

    if ( ! ( Test-Path function:Log-Information ) ) {
        . $PSScriptRoot/Logger.ps1
    }

    $Command = $args[0]
    $CommandArgs = @()

    if ( $args.Count -gt 1) {
        $CommandArgs = $args[1..($args.Count - 1)]
    }

    $_EAP = $ErrorActionPreference
    $ErrorActionPreference = "Continue"

    Log-Debug "Invoke-External: ${Command} ${CommandArgs}"

    & $command $commandArgs
    $Result = $LASTEXITCODE

    $ErrorActionPreference = $_EAP

    if ( $Result -ne 0 ) {
        throw "${Command} ${CommandArgs} exited with non-zero code ${Result}."
    }
}

function Compare-Stacks
{
    param(
        [Parameter(Mandatory = $true)]
        [System.Management.Automation.PathInfoStack] $Stack1,
        [Parameter(Mandatory = $true)]
        [System.Management.Automation.PathInfoStack] $Stack2
    )

    if ($Stack1 -eq $null -or $Stack2 -eq $null)
    {
        return $false
    }

    # Check if both stacks have the same number of elements
    if ($Stack1.Count -ne $Stack2.Count) {
        return $false
    }

    $StackArray1 = $Stack1.ToArray()
    $StackArray2 = $Stack2.ToArray()

    # Compare each PathInfo object in the stacks
    for ($i = 0; $i -lt $StackArray1.Count; $i++) {
        if ($StackArray1[$i].Path -ne $StackArray2[$i].Path) {
            return $false
        }
    }

    return $true
}

function Pop-Stack
{
    <#
        .SYNOPSIS
            Ensures we pop the whole Location stack provided.
        .DESCRIPTION
            If specified directory exists, switch to it. Otherwise create it,
            then switch.
        .EXAMPLE
            Ensure-Location "My-Directory"
            Ensure-Location -Path "Path-To-My-Directory"
    #>

    param(
        [Parameter(Mandatory = $true)]
        [string] $StackName,
        [Parameter(Mandatory=$false)]
        [System.Management.Automation.PathInfoStack] $InitialLocation = $null
    )

    if((Get-Location -StackName $StackName -ErrorAction 'SilentlyContinue') -eq $null){
        throw "Provided stack does not exist."
    }

    Log-Group "Popping Stack $StackName" -Silent
    do {
        $currentLocation = (Get-Location -StackName $StackName  -ErrorAction 'SilentlyContinue')

        #Early exit
        if (($InitialLocation -ne $null) -and (Compare-Stacks $currentLocation $InitialLocation))
        {
            #stop popping
            # Write-Debug "Will break"
            break
        }

        if($currentLocation -ne $null){
            Write-Debug "Popping $($currentLocation.Peek().Path)"
        }
        Pop-Location -Stack $StackName -ErrorAction 'SilentlyContinue'
    } while (($currentLocation.Count -gt 0))
    Log-Group
}

function Install 
{
    #Getting the current state of the BuildTemp stack. In case the stack was in use prior to this cmdlet. It will stay $null if stack didn't exist
    $initialLocation = (Get-Location -StackName BuildTemp -ErrorAction SilentlyContinue)

    Write-Debug "Pushing $(Get-Location)"
    Push-Location -Stack BuildTemp

    trap {
        #Makes sure to pop all locations in the stack. 
        Pop-Stack -StackName BuildTemp -InitialLocation $initialLocation
        Write-Error $_
        Log-Group
        exit 2
    }

    $ScriptHome = $PSScriptRoot
    $ProjectRoot = Resolve-Path -Path "$PSScriptRoot/.."
    $BuildSpecFile = "${ProjectRoot}/buildspec.json"

    $BuildSpec = Get-Content -Path ${BuildSpecFile} -Raw | ConvertFrom-Json
    $ProductName = $BuildSpec.name
    $ProductVersion = $BuildSpec.version
    
    Ensure-Location $ProjectRoot
    Write-Debug "Pushing $(Get-Location)"
    Push-Location -Stack BuildTemp #{ProjectRoot}

    $CmakeInstallArgs = @()

    $Preset = "windows-${Target}"

    $CmakeInstallArgs += @(
        '--install', "build_${Target}"
        '--config', $Configuration
    )

    if ( $VerbosePreference -eq 'Continue' ) {
        $CmakeInstallArgs += ('--verbose')
    }

    if($Intent -ne 'Testing'){
        $CmakeInstallArgs += ('--prefix', "${ProjectRoot}/release/${Configuration}")
    }

    Log-Group "Install ${ProductName}..."
    Invoke-External cmake @CmakeInstallArgs
    Log-Group

    if($Intent -eq 'Testing')
    {
        Pop-Stack -StackName BuildTemp -InitialLocation $initialLocation
        exit 0
    }

    Ensure-Location -Path "${ProjectRoot}/release"
    Write-Debug "Pushing $(Get-Location)"
    Push-Location -Stack BuildTemp

    $OutputName = "${ProductName}-${ProductVersion}-windows-${Target}"

    if($Intent -eq "Archive")
    {
        Log-Group "Archiving ${ProductName}..."
        $CompressArgs = @{
            Path = (Get-ChildItem -Path "${Configuration}" -Exclude "${OutputName}*.*")
            CompressionLevel = 'Optimal'
            DestinationPath = "${OutputName}.zip"
            Verbose = ($Env:CI -ne $null)
        }
        Compress-Archive -Force @CompressArgs
        Log-Group
    }
    elseif($Intent -eq "Installer")
    {
        Log-Group "Packaging ${ProductName}..."

        $IsccFile = "${ProjectRoot}/build_${Target}/installer-Windows.generated.iss"
        if ( ! ( Test-Path -Path $IsccFile ) ) {
            throw 'InnoSetup install script not found. Run the build script or the CMake build and install procedures first.'
        }

        #####
        #The only way I found that works reliably even if run multiple consecutive times
        New-Item -Path Package -ItemType Directory -ErrorAction "SilentlyContinue"
        Copy-Item -Path "${Configuration}\*" -Destination "Package/"  -ErrorAction 'SilentlyContinue'
        #####
        Invoke-External iscc ${IsccFile} /O"." /F"${OutputName}-Installer"
        Remove-Item -Path Package -Recurse
        Log-Group
    }

    Pop-Stack -StackName BuildTemp -InitialLocation $initialLocation
}


if ($PSCmdlet.MyInvocation.BoundParameters.ContainsKey('Debug'))
{
    try
    {
        Write-Debug "Running Debug Mode"
        Install
    }
    catch
    {
        Write-Error ($_.Exception | Format-List -Force | Out-String) -ErrorAction Continue
        Write-Error ($_.InvocationInfo | Format-List -Force | Out-String) -ErrorAction Continue
        throw
    }
}
else{
    Install
}