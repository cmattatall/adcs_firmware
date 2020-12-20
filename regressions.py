# FAST WAY TO RUN THE REGRESSION TESTS
import os
import shutil

if __name__ == "__main__":
    build_dir = "build"
    current_dir = os.getcwd()
    if os.path.exists(build_dir):
        shutil.rmtree(build_dir)
    os.system("cmake -S . -B %s -DBUILD_TESTING=ON" % (build_dir))
    os.system("cmake --build %s" % (build_dir))
    os.chdir("%s" % (build_dir))
    os.system("ctest -V")
    
    # NOW REPEAT BUT FOR CROSS COMPILE
    