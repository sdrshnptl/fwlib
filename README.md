# Fanuc FOCAS Library



## ChatGPT

### Clone the Repository:
First, clone the strangesast/fwlib repository to your local system:

```bash
git clone https://github.com/strangesast/fwlib.git
```


### Navigate to the Repository Directory:
Enter the repository directory:

```bash
Copy code
cd fwlib
```

### Identify the Library Files:

Within the repository, identify the appropriate library files based on your platform. It seems the instructions indicate that you may need to link or rename the appropriate libfwlib*.so files.

### Create Symbolic Links:
For example, let's say you're on Linux x86_64 and you want to create symbolic links for the library files:


#### For ubuntu
```bash
ln -s libfwlib32-linux-x64.so.1.0.5 libfwlib32.so.1
ln -s libfwlib32-linux-x64.so.1.0.5 libfwlib32.so
```
### for Raspberry pi
```bash
ln -s libfwlib32-linux-armv7.so.1.0.5 libfwlib32.so.1
ln -s libfwlib32-linux-armv7.so.1.0.5 libfwlib32.so
```
Replace libfwlib32-linux-x64.so.1.0.5 with the actual name of the library file you have in the repository. These commands create symbolic links libfwlib32.so.1 and libfwlib32.so pointing to libfwlib32-linux-x64.so.1.0.5.

### Verify Symbolic Links:
You can verify that the symbolic links have been created successfully using the ls command:

```bash
ls -l
```

This command will list the files in the directory, showing the symbolic links and their target files.

## Linux
From the root of this repository:  
```
gcc -L./ -Wl,-rpath . examples/c-minimal/main.c -lfwlib32 -lm -lpthread -o fanuc_minimal
./fanuc_minimal
```



[![Docker Hub](https://img.shields.io/docker/v/strangesast/fwlib?sort=date)](https://hub.docker.com/r/strangesast/fwlib)  
Header and runtime files for CNC communication  

# Docker
Build the base image with `docker build .`  

Build an example with `docker build examples/c/Dockerfile`   

# `examples/`  
Link or rename appropriate `libfwlib\*.so` (based on platform) to `libfwlib32.so.1` and `libfwlib32.so` 

On linux x86\_64 for example: `ln -s libfwlib32-linux-x64.so.1.0.5 libfwlib32.so` 

More instructions in each example folder



```
sudo dpkg --add-architecture armhf
sudo apt-get update
sudo apt-get install -y libc6-dev:armhf gcc-arm-linux-gnueabihf
#arm-linux-gnueabihf-gcc your_file.c -L./ -Wl,-rpath=./ -lpthread -lfwlib32 -o your_binary
#gcc -L./ -Wl,-rpath . examples/c-minimal/test_offset.c -lfwlib32 -lm -lpthread -o test_offset
arm-linux-gnueabihf-gcc examples/c-minimal/test_offset.c -L./ -Wl,-rpath=./ -lpthread -lfwlib32 -o test_offset


./your_binary
```


To make the **`LD_LIBRARY_PATH`** setting permanent, you can add it to one of the shell configuration files. This will ensure that the environment variable is set each time you open a new terminal session.

### Steps to Make `LD_LIBRARY_PATH` Permanent:

1. **Open the Shell Configuration File**:
   Depending on which shell you are using, you need to modify the appropriate configuration file. Here's how to find the correct file:
   - For **Bash** (default shell on most Linux distributions):
     - Edit `~/.bashrc` for terminal sessions.
     - If you're modifying the environment for login shells (such as when using SSH), use `~/.bash_profile` or `~/.profile`.

2. **Add the `export` Line**:
   Open the shell configuration file using a text editor like `nano`, `vim`, or `gedit`. For example, using `nano`:

   ```bash
   nano ~/.bashrc
   ```

3. **Add the Following Line** to the End of the File:

   ```bash
   export LD_LIBRARY_PATH=/home/sdrshnptl/fwlib:$LD_LIBRARY_PATH
   ```

4. **Save and Exit**:
   - In `nano`, save by pressing `CTRL + O`, then exit by pressing `CTRL + X`.
   - In `vim`, press `ESC`, type `:wq`, then hit `ENTER`.

5. **Apply the Changes**:
   To apply the changes immediately without restarting your terminal, run the following command:

   ```bash
   source ~/.bashrc
   ```

   This will reload the configuration file and apply the `LD_LIBRARY_PATH` setting for the current terminal session.

### Verifying the Change:

To verify that `LD_LIBRARY_PATH` is set correctly, run:

```bash
echo $LD_LIBRARY_PATH
```

You should see `/home/sdrshnptl/fwlib` included in the output.

This will now make the setting permanent and ensure that the **FANUC FOCAS** library is available for future terminal sessions without needing to manually set the path each time.

