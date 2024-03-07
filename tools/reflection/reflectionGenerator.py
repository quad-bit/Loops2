# import OS module
import os
import subprocess
import shutil

currentPath = os.getcwd()

shaderPath = currentPath +"//..//..//assets//graphics//vulkan//Shaders"
spvPath =  currentPath +"//..//..//assets//graphics//vulkan//Spvs"
reflPath = currentPath +"//..//..//assets//graphics//vulkan//Reflections"

def CleanFolder(folder):
    for filename in os.listdir(folder):
        file_path = os.path.join(folder, filename)
        try:
            if os.path.isfile(file_path) or os.path.islink(file_path):
                os.unlink(file_path)
            elif os.path.isdir(file_path):
                shutil.rmtree(file_path)
        except Exception as e:
            print('Failed to delete %s. Reason: %s' % (file_path, e))

def GenerateSpv():
    dir_list = os.listdir(shaderPath)
    commandBase = "glslangValidator.exe -V "
    for filename in dir_list:
        outputFilename = filename + ".spv"
        command = commandBase + shaderPath + "//" + filename + " -o " + spvPath+ "//" + outputFilename
        # print(command)
        s = subprocess.check_call(command, shell=True) 
        # print(", return code", s) 

def GenerateReflections():
    dir_list = os.listdir(spvPath)
    commandBase = "spirv-cross.exe "
    for filename in dir_list:
        outputFilename = filename + ".refl"
        outputFilename = outputFilename.replace('.spv', '')
        command = commandBase + spvPath + "//" + filename + " --reflect --output " + reflPath+ "//" + outputFilename
        print(command)
        s = subprocess.check_call(command, shell=True) 

CleanFolder(reflPath)
CleanFolder(spvPath)

GenerateSpv()
GenerateReflections()


