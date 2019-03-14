# InstanceMan

## Getting Started

### Prerequisites

* Cinema 4D R20
* Cinema 4D R20 C++ SDK
* Visual Studio 2015
* Cinema 4D SDK Project Tool

Some experience building Cinema 4D plugins. If you haven't before [this post](https://plugincafe.maxon.net/topic/10967/compiling-the-cinema-4d-r20-c-sdk-examples/4) may be useful.

### Downloading

1. Open **Command Prompt**.
1. Navigate to your C4D plugins directory for me that's `cd C:\Users\donovan\MAXON\plugins`
1. Clone this repo: `git clone git@github.com:mp5gosu/InstanceMan.git`

### Building

#### Update the Project

1. Copy the `frameworks` folder from the C4D SDK into the `InstanceMan` directory.
1. Update the plugin project with the C4D SDK Project Tool `kernel_app_64bit.exe g_updateproject=C:\Users\YourUserNameHere\MAXON\plugins\InstanceMan`.

#### Build the Project

1. Open the `instanceman.vcxproj` file in the `project` folder that was generated/updated by the Project Tool in Visual Stu.
1. Build > Build Project

#### Verify the Build

1. Open Cinema 4D
1. Look in the Plugins pulldown for "instanceman"
