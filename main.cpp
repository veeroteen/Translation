#include <iostream>
#include "Tables.h"
#include <stdexcept>
#include <fstream>
int main()
{

    std::string testFolder = "test1/";
    StaticTable keywords;
    {
        std::ifstream stream(testFolder + "Keywords.txt");
        std::string buff = "";
        std::vector<std::string> bArr;
        while (std::getline(stream, buff))
        {
            bArr.push_back(buff);
        }
        keywords.load(std::move(bArr));
    }
    StaticTable operators;
    {
        std::ifstream stream(testFolder + "Operators.txt");
        std::string buff = "";
        std::vector<std::string> bArr;
        while (std::getline(stream, buff))
        {
            bArr.push_back(buff);
        }
        operators.load(std::move(bArr));
    }
    StaticTable separators;
    {
        std::ifstream stream(testFolder + "Separators.txt");
        std::string buff = "";
        std::vector<std::string> bArr;
        while (std::getline(stream, buff))
        {
            bArr.push_back(buff);
        }
        separators.load(std::move(bArr));
    }

    DynamicTable constants;
    DynamicTable identificators;

    int command = 0;
    std::string buf = "";
    bool flag = true;
    while (flag)
    {
        std::cout << "\nDynamic Table Terminal\n";
        std::cout << "[1] Add variable\n";
        std::cout << "[2] Add constant\n";
        std::cout << "[3] Find index by name\n";
        std::cout << "[4] Find var by index\n";
        std::cout << "[5] Find const by index\n";
        std::cout << "[6] Prove the element exitence\n";
        std::cout << "[7] Write down a static table\n";
        std::cout << "[0] Exit\n";
        std::cout << "Command: \n";
        std::cin >> command;

        switch (command)
        {
        case 0:
        {
            flag = false;
            break;
        }
        case 1:
        {
            std::cout << "Element name: ";
            std::cin >> buf;
            if (identificators.contains(buf))
            {
                std::cout << "Such element already exists :(\n";
                break;
            }
            std::cout << "Value: ";
            if (!(std::cin >> command))
            {
                std::cout << "It's not an int value! :o\n";
                std::cin.ignore();
                std::cin.clear();
                break;
            }
            std::cout << "Element added at index " << identificators.add(buf, new LexemeAttributes(static_cast<void*>(new int(command)))) << std::endl;
            break;
        }
        case 2:
        {
            std::cout << "Element name: ";
            std::cin >> buf;
            if (constants.contains(buf))
            {
                std::cout << "Such element already exists :(\n";
                break;
            }
            std::cout << "Value: ";
            if (!(std::cin >> command))
            {
                std::cout << "It's not an int value! :o\n";
                std::cin.ignore();
                std::cin.clear();
                break;
            }
            std::cout << "Element added at index " << identificators.add(buf, new LexemeAttributes(static_cast<void*>(new int(command)))) << std::endl;
            break;
        }
        case 3:
        {
            std::cout << "Element name: ";
            std::cin >> buf;
            if (identificators.contains(buf))
            {
                std::cout << "Found a variable: " << identificators.find(buf)->second << std::endl;
                break;
            }
            if (constants.contains(buf))
            {
                std::cout << "Found a constant: " << constants.find(buf)->second << std::endl;
                break;
            }
            std::cout << "No such element found :(\n";
            break;
        }
        case 4:
            std::cout << "Element index: ";
            std::cin >> command;
            if (command >= identificators.size())
                std::cout << "No such element found :(\n";
            else
            {
                std::cout << "Name: " << identificators.getVal(command) << std::endl;
                std::cout << "Value: " << identificators.getAttributes(command) << std::endl;
            }
            break;
        case 5:
            std::cout << "Element index: ";
            std::cin >> command;
            if (command >= constants.size())
                std::cout << "No such element found :(\n";
            else
            {
                std::cout << "Name: " << constants.getVal(command) << std::endl;
                std::cout << "Value: " << constants.getAttributes(command) << std::endl;
            }
            break;
        case 6:
        {
            std::cout << "Element name: ";
            std::cin >> buf;

            if (identificators.contains(buf))
                std::cout << "A variable exists! :)\n";
            else if (constants.contains(buf))
                std::cout << "A constant exists! :)\n";
            else
                std::cout << "Element doesn't exist! :(\n";
            break;
        }
        case 7:
        {
            std::cout << "[1] Operators\n";
            std::cout << "[2] Separators\n";
            std::cout << "[3] Key words\n";
            std::cout << "[0] Cancel\n";
            if (!(std::cin >> command))
            {
                std::cout << "Don't try to break the program, pls :(\n";
                std::cin.ignore();
                std::cin.clear();
                break;
            }
            std::cout << "\n";
            switch (command)
            {
            case 0:
                break;
            case 1:
                operators.drawTable();
                break;
            case 2:
                separators.drawTable();
                break;
            case 3:
                keywords.drawTable();
                break;
            default:
                std::cout << "Unknown table :(\n";
                break;
            }
            break;
        }
        default:
        {
            std::cout << "Unknown command. Bad luck :(\n";
            break;
        }
        }
    }
}