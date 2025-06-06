import os
import subprocess
import platform

from SetupPython import PythonConfiguration as PythonRequirements

# Make sure everything we need for the setup is installed
PythonRequirements.Validate()

from SetupVulkan import VulkanConfiguration as VulkanRequirements

os.chdir('./../') # Change from devtools/scripts directory to root

VulkanRequirements.Validate()

if platform.system() == "Windows":
    print("\nRunning premake...")
    bat_path = os.path.abspath("./scripts/Win-GenProjects.bat")
    print(f"Attempting to run: {bat_path}")
    subprocess.call([os.path.abspath("./scripts/Win-GenProjects.bat"), "nopause"])
    

print("\nSetup completed!")