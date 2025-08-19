#define DICT_IMPLEMENTATION
#include "core/stl/dict.h"
#include "core/c_test.h"

TEST(dict, test) {
    /* 1. 新建 & 空表查询 */
    dict_t *d = dict_new();
    ASSERT_TRUE(d != NULL);
    ASSERT_STREQ(dict_get(d, "missing", "fallback"), "fallback");

    /* 2. 增/改/查 */
    dict_add(d, "key1", "val1");
    dict_add(d, "key2", "val2");
    ASSERT_STREQ(dict_get(d, "key1", ""), "val1");
    ASSERT_STREQ(dict_get(d, "key2", ""), "val2");

    /* 覆盖写入 */
    dict_add(d, "key1", "new1");
    ASSERT_STREQ(dict_get(d, "key1", ""), "new1");

    /* 3. 删除 */
    ASSERT_EQ(dict_del(d, "key1"), 0);
    ASSERT_STREQ(dict_get(d, "key1", "gone"), "gone");
    ASSERT_EQ(dict_del(d, "key1"), -1);          /* 不存在 */

    /* 4. 枚举 */
    dict_add(d, "kA", "vA");
    dict_add(d, "kB", "vB");
    int pos = 0;
    const char *k, *v;
    int cnt = 0;
    while ((pos = dict_enum(d, pos, &k, &v)) != -1) {
        ASSERT_TRUE(k != NULL);
        ASSERT_TRUE(v != NULL);
        ++cnt;
    }
    ASSERT_EQ(cnt, 3);   /* key2 + kA + kB */

    /* 5. 扩容：插入大量键值 */
    const int N = 100;
    for (int i = 0; i < N; ++i) {
        char key[32], val[32];
        snprintf(key, sizeof(key), "k%d", i);
        snprintf(val, sizeof(val), "v%d", i);
        ASSERT_EQ(dict_add(d, key, val), 0);
    }
    for (int i = 0; i < N; ++i) {
        char key[32], expect[32];
        snprintf(key,   sizeof(key),   "k%d", i);
        snprintf(expect,sizeof(expect),"v%d", i);
        ASSERT_STREQ(dict_get(d, key, ""), expect);
    }

    /* 6. 清空并释放 */
    dict_free(d);
}