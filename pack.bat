rmdir temp\bloodworks /s /q
mkdir temp
mkdir temp\bloodworks

xcopy game\*.txt .\temp\bloodworks /sy
xcopy game\*.json .\temp\bloodworks /sy
xcopy game\*.lua .\temp\bloodworks /sy
xcopy game\*.dll .\temp\bloodworks /sy
xcopy game\*.png .\temp\bloodworks /sy
xcopy game\*.ogg .\temp\bloodworks /sy
xcopy game\*.wav .\temp\bloodworks /sy
xcopy game\*.exe .\temp\bloodworks /sy
xcopy game\*.ps .\temp\bloodworks /sy
xcopy game\*.vs .\temp\bloodworks /sy
xcopy game\*.html .\temp\bloodworks /sy

del .\temp\bloodworks\config.txt
del .\temp\bloodworks\user_details.txt
del .\temp\bloodworks\keys.txt
del .\temp\bloodworks\stdout.txt
del .\temp\bloodworks\stdout_to_send.txt
del .\temp\bloodworks\uploader_stdout.txt

rmdir .\temp\bloodworks\resources\mods /s /q
mkdir .\temp\bloodworks\resources\mods
mkdir .\temp\bloodworks\resources\temp

del bloodworks.zip

powershell.exe -nologo -noprofile -command "& { Add-Type -A 'System.IO.Compression.FileSystem'; [IO.Compression.ZipFile]::CreateFromDirectory('temp', 'bloodworks.zip'); }"
