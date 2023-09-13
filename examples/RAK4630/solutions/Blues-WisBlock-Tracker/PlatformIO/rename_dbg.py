import datetime
import shutil
import os

if os.path.exists("./Debug-Build"):
	shutil.rmtree("./Debug-Build")

Import("env")

my_flags = env.ParseFlags(env['BUILD_FLAGS'])
defines = {k: v for (k, v) in my_flags.get("CPPDEFINES")}

build_tag = "RAK_"

version_tag_1 = defines.get("SW_VERSION_1")
version_tag_2 = defines.get("SW_VERSION_2")
version_tag_3 = defines.get("SW_VERSION_3")
build_date = datetime.datetime.now().strftime('%Y.%m.%d.%H.%M.%S')

env.Replace(PROGNAME="../../../Debug-Build/RAK-Blues-Tracker_V%s.%s.%s_%s_dbg" % (version_tag_1,version_tag_2,version_tag_3,build_date))
