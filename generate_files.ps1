$fileName = ""
$basePath = $PSScriptRoot

$folderName = $fileName -replace "\.[^.]+$" # Remove file extension
$folderPath = Join-Path -Path $basePath -ChildPath $folderName

$srcPath = Join-Path -Path $folderPath -ChildPath "src"
$dataPath = Join-Path -Path $folderPath -ChildPath "data"
$docsPath = Join-Path -Path $folderPath -ChildPath "docs"
$testsPath = Join-Path -Path $folderPath -ChildPath "_tests_"

# Create the main folder
New-Item -ItemType Directory -Path $folderPath

# Create the subfolders
New-Item -ItemType Directory -Path $srcPath
New-Item -ItemType Directory -Path $dataPath
New-Item -ItemType Directory -Path $docsPath
New-Item -ItemType Directory -Path $testsPath