#include <stdio.h>
#include <portable-file-dialogs.h>


int main(int argc, char **argv)
{
    auto f = pfd::open_file("Choose files to read", "/tmp/",
        { "Text Files (.txt .text)", "*.txt *.text",
          "All Files", "*" },
        true);
    std::cout << "Selected files:";
    for (auto const &name : f.result())
        std::cout << " " + name;
    std::cout << "\n";
}