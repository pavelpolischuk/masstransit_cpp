param
(
    [string] $PBuildType,
    [string] $PInstallDir, #${CMAKE_BINARY_DIR}/
    [string] $PLibraryDependencies,
    [boolean] $PVerbose=$True
)

echo $PInstallDir
echo $PConfiguration

New-Item -Path $PInstallDir -ItemType Directory -Force
if($PBuildType.Equals("Debug"))
{
    if($PVerbose)
    {
        echo "using Debug dependencies"
    }
    New-Variable -Name CopyDependencyType -Value "debug"
}
else
{
    if($PVerbose)
    {
        echo "using Release dependencies"
    }
    New-Variable -Name CopyDependencyType -Value "optimized"
}

function CopyDependency([string]$PLibrary, 
                        [string]$PDestinationDir)
{
    if($PVerbose)
    {
        echo "coping $PLibrary -> $PDestinationDir"
    }
    if($PLibrary.EndsWith(".lib"))
    {
        $PLibrary = $PLibrary.Replace(".lib", ".dll");
    }

    if(Test-Path $PLibrary)
    {
        Copy-Item -Path $PLibrary -Destination $PInstallDir
    }
    else
    {
        if($PVerbose)
        {
            echo "unable to find $PLibrary"
        }
    }
    
}

function IsConfigSpecifier([string]$str)
{
    return $str.Equals("debug") -or $str.Equals("optimized")
}

$libraries = $PLibraryDependencies.Split(";")
[bool]$copy_next = $False
foreach($library in $libraries)
{
    if($(IsConfigSpecifier -str $library))
    {
        $copy_next = $library.Equals($CopyDependencyType)
    }
    else
    {
        if($copy_next)
        {
            CopyDependency -PLibrary $library -PDestinationDir $PInstallDir
        }
        $copy_next = $True
    }
}

