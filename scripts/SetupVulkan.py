import os
import sys
import subprocess
from pathlib import Path

import Utils

from io import BytesIO
from urllib.request import urlopen

class VulkanConfiguration:
    requiredVulkanVersion = "1.4."
    installVulkanVersion = "1.4.309.0"
    vulkanDirectory = "./Bacon/vendor/VulkanSDK"

    @classmethod
    def Validate(cls):
        if (not cls.CheckVulkanSDK()):
            print("Vulkan SDK not installed correctly.")
            return
            
        if (not cls.CheckVulkanSDKDebugLibs()):
            print("\nNo Vulkan SDK debug libs found. Install Vulkan SDK with debug libs.")
            print("Debug configuration disabled.")

    @classmethod
    def CheckVulkanSDK(cls):
        vulkanSDK = os.environ.get("VULKAN_SDK")
        if (vulkanSDK is None):
            print("\nYou don't have the Vulkan SDK installed!")
            cls.__InstallVulkanSDK()
            return False
        else:
            print(f"\nLocated Vulkan SDK at {vulkanSDK}")

        if (cls.requiredVulkanVersion not in vulkanSDK):
            print(f"You don't have the correct Vulkan SDK version! (Engine requires {cls.requiredVulkanVersion})")
            cls.__InstallVulkanSDK()
            return False
    
        print(f"Correct Vulkan SDK located at {vulkanSDK}")
        return True

    @classmethod
    def __InstallVulkanSDK(cls):
        permissionGranted = False
        while not permissionGranted:
            reply = str(input("Would you like to install VulkanSDK {0:s}? [Y/N]: ".format(cls.installVulkanVersion))).lower().strip()[:1]
            if reply == 'n':
                return
            permissionGranted = (reply == 'y')

        vulkanInstallURL = f"https://sdk.lunarg.com/sdk/download/{cls.installVulkanVersion}/windows/VulkanSDK-{cls.installVulkanVersion}-Installer.exe"
        vulkanPath = f"{cls.vulkanDirectory}/VulkanSDK-{cls.installVulkanVersion}-Installer.exe"
        print("Downloading {0:s} to {1:s}".format(vulkanInstallURL, vulkanPath))
        Utils.DownloadFile(vulkanInstallURL, vulkanPath)
        print("Running Vulkan SDK installer...")
        os.startfile(os.path.abspath(vulkanPath))
        print("Re-run this script after installation!")
        quit()

    @classmethod
    def CheckVulkanSDKDebugLibs(cls):
        vulkanSDK = os.environ.get("VULKAN_SDK")
        shadercdLib = Path(f"{vulkanSDK}/Lib/shaderc_sharedd.lib")

        return shadercdLib.exists()


#     @classmethod
#     def CheckVulkanSDKDebugLibs(cls):
#         # Target paths
#         target_dir = Path(cls.vulkanDirectory) / "Lib"
#         target_file = target_dir / "shaderc_sharedd.lib"
#         if target_file.exists():
#             print("[OK] Debug libraries already exist")
#             return True

#         # Search Vulkan SDK installation directory
#         vulkan_sdk = os.environ.get("VULKAN_SDK")
#         if not vulkan_sdk:
#             print("[ERR] Vulkan SDK path not found. Install SDK first.")
#             return False

#         # Check common debug library paths
#         source_paths = [
#             Path(vulkan_sdk) / "Lib" / "shaderc_sharedd.lib",
#             Path(vulkan_sdk) / "Debug" / "Lib" / "shaderc_sharedd.lib"
#         ]
#         for path in source_paths:
#             if path.exists():
#                 target_dir.mkdir(parents=True, exist_ok=True)
#                 shutil.copy(path, target_dir)
#                 print(f"[OK] Copied debug library from: {path}")
#                 return True

#         # Fallback instructions
#         print(f"""
# [ERR] Debug library file 'shaderc_sharedd.lib' not found.
# Manual steps required:
# 1. Open Vulkan SDK directory: {vulkan_sdk}
# 2. Locate the file in Lib/Debug or similar subdirectories
# 3. Copy it to: {target_dir}
#         """)
#         return False

if __name__ == "__main__":
    VulkanConfiguration.Validate()