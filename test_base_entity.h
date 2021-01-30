#ifndef TEST_BASE_ENTITY_H
#define TEST_BASE_ENTITY_H
#include "base_entity.h"

class test_base_entity: IBaseJsonEntity<int, std::string, int> {
public:
    test_base_entity(): IBaseJsonEntity<int, std::string, int>({"id", "name", "value"}) {

    }
};

#endif // TEST_BASE_ENTITY_H
