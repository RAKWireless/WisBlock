import shutil
import os
import os.path
from os import path

def copy_files(src_1, src_2, dest_1, dest_2, bsp):
	if path.exists(dest_1):
		shutil.copy(src_1, dest_1)
		if path.exists(dest_2):
			shutil.rmtree(dest_2)
		shutil.copytree(src_2, dest_2)
		print("Patched " + bsp)
	else:
		print("Could not find " + dest_1)
 
def copy_files_ext(src_1, src_2, src_3, dest_1, dest_2, dest_3, bsp):
	if path.exists(dest_1):
		shutil.copy(src_1, dest_1)
		if path.exists(dest_2):
			shutil.rmtree(dest_2)
		shutil.copytree(src_2, dest_2)
		try:
			os.remove(dest_3+"/Print.h")
			os.remove(dest_3+"/Print.cpp")
		except OSError as e:  ## if failed, report it back to the user ##
			print ("File not found.")
		shutil.copyfile(src_3+"/Print.h", dest_3+"/Print.h")
		shutil.copyfile(src_3+"/Print.cpp", dest_3+"/Print.cpp")
		print("Patched " + bsp)
	else:
		print("Could not find " + dest_1)
 
# path to RAK4631 source directories
src_dir1 = './platforms/nordicnrf52/boards/wiscore_rak4631.json'
src_dir2 = './packages/framework-arduinoadafruitnrf52/variants/WisCore_RAK4631_Board'

# path to RAK4631 destination directory
dest_dir1 = '../platforms/nordicnrf52/boards'
dest_dir2 = '../packages/framework-arduinoadafruitnrf52/variants/WisCore_RAK4631_Board'

print("Patching RAK4631")
copy_files(src_dir1, src_dir2, dest_dir1, dest_dir2, "RAK4631")

# path to RAK11200 source directories
src_dir1 = './platforms/espressif32/boards/wiscore_rak11200.json'
src_dir2 = './packages/framework-arduinoespressif32/variants/WisCore_RAK11200_Board'

# path to RAK11200 destination directory
dest_dir1 = '../platforms/espressif32/boards'
dest_dir2 = '../packages/framework-arduinoespressif32/variants/WisCore_RAK11200_Board'

print("Patching RAK11200")
copy_files(src_dir1, src_dir2, dest_dir1, dest_dir2, "RAK11200")

# path to RAK11310 source directories
src_dir1 = './platforms/raspberrypi/boards/rak11300.json'
src_dir2 = './packages/framework-arduino-mbed/variants/WisBlock_RAK11300_Board'
src_dir3 = './packages/framework-arduino-mbed/cores/arduino/api'

# path to RAK11310 destination directory
dest_dir1 = '../platforms/raspberrypi/boards'
dest_dir2 = '../packages/framework-arduino-mbed/variants/WisBlock_RAK11300_Board'
dest_dir3 = '../packages/framework-arduino-mbed/cores/arduino/api'

print("Patching RAK11310")
copy_files_ext(src_dir1, src_dir2, src_dir3, dest_dir1, dest_dir2, dest_dir3, "RAK11310")

# # path to RAK4260 source directories
# src_dir1 = './platforms/atmelsam/boards/rak4260.json'
# src_dir2 = './packages/framework-arduino-samd/variants/rak4260'
# src_dir3 = './packages/framework-arduino-samd/cores/arduino/api'

# # path to RAK4260 destination directory
# dest_dir1 = '../platforms/atmelsam/boards/rak4260.json'
# dest_dir2 = '../packages/framework-arduino-samd/variants/rak4260'
# dest_dir3 = '../packages/framework-arduino-samd/cores/arduino/api'

# print("Patching RAK4260")
# copy_files_ext(src_dir1, src_dir2, src_dir3, dest_dir1, dest_dir2, dest_dir3, "RAK4260")

print("Patch done, check output if successful")

