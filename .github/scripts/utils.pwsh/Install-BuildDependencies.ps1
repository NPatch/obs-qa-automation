function Install-BuildDependencies {
    <#
        .SYNOPSIS
            Installs required build dependencies.
        .DESCRIPTION
            Additional packages might be needed for successful builds. This module contains additional
            dependencies available for installation via winget and, if possible, adds their locations
            to the environment path for future invocation.
        .EXAMPLE
            Install-BuildDependencies
    #>

    param(
        [string] $WingetFile = "$PSScriptRoot/.Wingetfile"
    )

    if ( ! ( Test-Path function:Log-Warning ) ) {
        . $PSScriptRoot/Logger.ps1
    }

    $Prefixes = @{
        'x64' = ${Env:ProgramFiles}
        'x86' = ${Env:ProgramFiles(x86)}
        'arm64' = ${Env:ProgramFiles(arm)}
        'appdata-x64' = ${Env:LOCALAPPDATA}+"\Programs"
    }

    $Paths = $Env:Path -split [System.IO.Path]::PathSeparator

    $WingetOptions = @('install', '--accept-package-agreements', '--accept-source-agreements')

    if ( $script:Quiet ) {
        $WingetOptions += '--silent'
    }

    Log-Group 'Check Windows build requirements'
    Get-Content $WingetFile | ForEach-Object {
        $_, $Package, $_, $Path, $_, $Binary, $_, $Version = $_ -replace ',','' -split " +(?=(?:[^\']*\'[^\']*\')*[^\']*$)" -replace "'",''

        $Prefixes.GetEnumerator() | ForEach-Object {
            $Prefix = $_.value
            $FullPath = "${Prefix}\${Path}"
            Log-Debug "Checking for ${FullPath}"
            if ( ( Test-Path $FullPath  ) -and ! ( $Paths -contains $FullPath ) ) {
                $Env:Path += [System.IO.Path]::PathSeparator+$FullPath
            }
        }

        Log-Debug "Checking for command ${Binary}"
        $Found = Get-Command -ErrorAction SilentlyContinue $Binary

        if ( $Found ) {
            Log-Status "Found dependency ${Binary} as $($Found.Source)"
        } else {
            Log-Status "Installing package ${Package} $(if ( $Version -ne $null ) { "Version: ${Version}" } )"

            if ( $Version -ne $null ) {
                $WingetOptions += @('--version', ${Version})
            }

            try {
                $Params = $WingetOptions + $Package

                Log-Status "Executing winget ${Params}"
                winget @Params
                
                if($Binary){
                    Log-Status "Adding ${Package} to PATH ${FullPath}"
                    $Prefixes.GetEnumerator() | ForEach-Object {
                        $Prefix = $_.value
                        $FullPath = "${Prefix}\${Path}"
                        if ( ( Test-Path $FullPath  ) -and ! ( $Paths -contains $FullPath ) ) {
                            $current_env_path = [Environment]::GetEnvironmentVariable("PATH", [EnvironmentVariableTarget]::User)
                            $new_env_path = $current_env_path
                            if( !$new_env_path.EndsWith(";")){
                                $new_env_path += ${[System.IO.Path]::PathSeparator}
                            }
                            $new_env_path += $FullPath
                            [Environment]::SetEnvironmentVariable("PATH",$new_env_path, [EnvironmentVariableTarget]::User)
                        }
                    }
                }
            } catch {
                throw "Error while installing winget package ${Package}: $_"
            }
        }
    }
    Log-Group
}
