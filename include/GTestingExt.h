
#ifndef __TESTINGEXT_H__
#define __TESTINGEXT_H__

#define TE_GET_TESTING_NAME(suite,case) test_##suite##case

#define DEF_TEST_INH(suite,case,base)                       \
    class TE_GET_TESTING_NAME(suite,case) : public base {  \
    public:                                                 \
        virtual void TestBody();                            \
    };                                                      \
    void TE_GET_TESTING_NAME(suite,case)::TestBody()        \



#define DIS_TEST(suite,case)                    \
    DEF_TEST_INH(suite,case,::testing::Test)

#define DIS_TEST_F(suite,case) \
    DEF_TEST_INH(suite,case,suite)


#endif /* __TESTINGEXT_H__ */