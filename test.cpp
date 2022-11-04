#include <unistd.h>

#include <iostream>
#include <string>

namespace test {
class Hey {
   public:
    enum TEST { HALLO, BLUB };
};

}  // namespace test

int main() {
    test::Hey::TEST hallo = test::Hey::TEST::HALLO;

    if (hallo == test::Hey::HALLO) {
        std::cout << "hallo" << std::endl;
    } else {
        std::cout << "blub" << std::endl;
    }
    return 0;
}
