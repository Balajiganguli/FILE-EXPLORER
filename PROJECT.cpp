#include <iostream>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstring>
#include <iomanip>
#include <vector>
#include <fstream>

using namespace std;

string getModeString(mode_t fileMode) {
    string modeString;
    modeString += (S_ISDIR(fileMode)) ? 'd' : '-';
    modeString += (fileMode & S_IRUSR) ? 'r' : '-';
    modeString += (fileMode & S_IWUSR) ? 'w' : '-';
    modeString += (fileMode & S_IXUSR) ? 'x' : '-';
    modeString += (fileMode & S_IRGRP) ? 'r' : '-';
    modeString += (fileMode & S_IWGRP) ? 'w' : '-';
    modeString += (fileMode & S_IXGRP) ? 'x' : '-';
    modeString += (fileMode & S_IROTH) ? 'r' : '-';
    modeString += (fileMode & S_IWOTH) ? 'w' : '-';
    modeString += (fileMode & S_IXOTH) ? 'x' : '-';
    return modeString;
}

void displayDirectory() {
    char pathBuffer[1024];
    getcwd(pathBuffer, sizeof(pathBuffer));

    DIR *directory;
    struct dirent *dirEntry;
    struct stat statInfo;

    directory = opendir(pathBuffer);
    if (!directory) {
        perror("Unable to open directory");
        return;
    }

    cout << "\nCurrent Directory: " << pathBuffer << endl;
    cout << "---------------------------------------------------------------\n";
    cout << setw(25) << left << "Name" << setw(12) << "Type" << "Permissions" << endl;
    cout << "---------------------------------------------------------------\n";

    while ((dirEntry = readdir(directory)) != nullptr) {
        if (strcmp(dirEntry->d_name, ".") == 0 || strcmp(dirEntry->d_name, "..") == 0)
            continue;

        string fullPath = string(pathBuffer) + "/" + dirEntry->d_name;
        stat(fullPath.c_str(), &statInfo);

        string fileType = (S_ISDIR(statInfo.st_mode)) ? "DIR" : "FILE";
        string filePerm = getModeString(statInfo.st_mode);

        cout << setw(25) << left << dirEntry->d_name << setw(12) << fileType << filePerm << endl;
    }

    closedir(directory);
}

void navigateTo(const string &dirName) {
    if (chdir(dirName.c_str()) == 0)
        cout << "Changed directory to: " << dirName << endl;
    else
        perror("Failed to change directory");
}

void copyData(const string &sourceFile, const string &targetFile) {
    ifstream srcStream(sourceFile, ios::binary);
    ofstream tgtStream(targetFile, ios::binary);

    if (!srcStream) {
        cout << "Source file not found!\n";
        return;
    }
    if (!tgtStream) {
        cout << "Failed to create target file!\n";
        return;
    }

    tgtStream << srcStream.rdbuf();
    cout << "File copied successfully!\n";
}

void relocateFile(const string &oldFile, const string &newFile) {
    if (rename(oldFile.c_str(), newFile.c_str()) == 0)
        cout << "File moved successfully!\n";
    else
        perror("Failed to move file");
}

void removeFile(const string &filePath) {
    if (remove(filePath.c_str()) == 0)
        cout << "File deleted successfully!\n";
    else
        perror("Failed to delete file");
}

void generateFile(const string &filePath) {
    ofstream newFile(filePath);
    if (newFile)
        cout << "File created: " << filePath << endl;
    else
        cout << "Failed to create file!\n";
    newFile.close();
}

void findFile(const string &rootPath, const string &targetName) {
    DIR *dirStream;
    struct dirent *dirEntry;
    dirStream = opendir(rootPath.c_str());
    if (!dirStream) return;

    while ((dirEntry = readdir(dirStream)) != nullptr) {
        if (strcmp(dirEntry->d_name, ".") == 0 || strcmp(dirEntry->d_name, "..") == 0)
            continue;

        string currentPath = rootPath + "/" + dirEntry->d_name;
        if (strstr(dirEntry->d_name, targetName.c_str()) != nullptr)
            cout << "Found: " << currentPath << endl;

        if (dirEntry->d_type == DT_DIR)
            findFile(currentPath, targetName);
    }
    closedir(dirStream);
}

void showFilePermissions(const string &fileName) {
    struct stat statBuf;
    if (stat(fileName.c_str(), &statBuf) == 0)
        cout << "Permissions for " << fileName << ": " << getModeString(statBuf.st_mode) << endl;
    else
        perror("Unable to fetch file info");
}

void modifyFilePermissions(const string &fileName, const string &permCode) {
    mode_t modeBits = 0;

    if (permCode.size() != 9) {
        cout << "Invalid format! Use rwxr-xr-- style.\n";
        return;
    }

    if (permCode[0] == 'r') modeBits |= S_IRUSR;
    if (permCode[1] == 'w') modeBits |= S_IWUSR;
    if (permCode[2] == 'x') modeBits |= S_IXUSR;
    if (permCode[3] == 'r') modeBits |= S_IRGRP;
    if (permCode[4] == 'w') modeBits |= S_IWGRP;
    if (permCode[5] == 'x') modeBits |= S_IXGRP;
    if (permCode[6] == 'r') modeBits |= S_IROTH;
    if (permCode[7] == 'w') modeBits |= S_IWOTH;
    if (permCode[8] == 'x') modeBits |= S_IXOTH;

    if (chmod(fileName.c_str(), modeBits) == 0)
        cout << "Permissions changed successfully!\n";
    else
        perror("Permission change failed");
}

int main() {
    char userChoice;
    string input1, input2;

    cout << "=== Terminal File Explorer ===" << endl;

    while (true) {
        displayDirectory();

        cout << "\nOptions:\n";
        cout << " [C] Change directory\n";
        cout << " [B] Go back\n";
        cout << " [1] Copy file\n";
        cout << " [2] Move file\n";
        cout << " [3] Delete file\n";
        cout << " [4] Create file\n";
        cout << " [5] Search file/folder\n";
        cout << " [6] View permissions\n";
        cout << " [7] Change permissions\n";
        cout << " [Q] Quit\n";
        cout << "Enter choice: ";
        cin >> userChoice;

        switch (userChoice) {
            case 'C': case 'c':
                cout << "Enter directory name: ";
                cin >> input1;
                navigateTo(input1);
                break;

            case 'B': case 'b':
                navigateTo("..");
                break;

            case '1':
                cout << "Enter source file: ";
                cin >> input1;
                cout << "Enter destination file: ";
                cin >> input2;
                copyData(input1, input2);
                break;

            case '2':
                cout << "Enter source file: ";
                cin >> input1;
                cout << "Enter destination file: ";
                cin >> input2;
                relocateFile(input1, input2);
                break;

            case '3':
                cout << "Enter file to delete: ";
                cin >> input1;
                removeFile(input1);
                break;

            case '4':
                cout << "Enter new file name: ";
                cin >> input1;
                generateFile(input1);
                break;

            case '5':
                cout << "Enter name to search: ";
                cin >> input1;
                {
                    char currentDir[1024];
                    getcwd(currentDir, sizeof(currentDir));
                    cout << "Searching in: " << currentDir << endl;
                    findFile(currentDir, input1);
                }
                break;

            case '6':
                cout << "Enter file name: ";
                cin >> input1;
                showFilePermissions(input1);
                break;

            case '7':
                cout << "Enter file name: ";
                cin >> input1;
                cout << "Enter permission string (rwxr-xr--): ";
                cin >> input2;
                modifyFilePermissions(input1, input2);
                break;

            case 'Q': case 'q':
                cout << "Exiting File Explorer. Goodbye!\n";
                return 0;

            default:
                cout << "Invalid choice. Try again.\n";
        }
    }
}
