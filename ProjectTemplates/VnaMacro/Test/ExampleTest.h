#ifndef EXAMPLE_TEST_H
#define EXAMPLE_TEST_H


// RsaToolbox
#include <TestClass.h>

class ExampleTest : public RsaToolbox::TestClass
{
public:
    TestClass();
    ~TestClass();

private slots:
    void test1();

};

#endif