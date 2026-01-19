#include "pch.h"

class SubsetsTest : public TestAbstract, public ::testing::Test 
{
protected:
    void SetUp() override 
    {
        global_ws_cmt->clear();
        if(K_RWS[COMMENTS][0])
        {
            delete K_RWS[COMMENTS][0];
            K_RWS[COMMENTS][0] = nullptr;
        }
    }

    void TearDown() override 
    {
        global_ws_cmt->clear();
        if(K_RWS[COMMENTS][0])
        {
            delete K_RWS[COMMENTS][0];
            K_RWS[COMMENTS][0] = nullptr;
        }
    }
};


TEST_F(SubsetsTest, Subset)
{
    std::string pattern = "A*";
    std::string modified = "modified";

    // GLOBAL KDB
    global_ws_cmt->load(str_input_test_dir + "fun.ac");
    std::string cmt_ACAF = std::string(global_ws_cmt->get_obj("ACAF"));
    std::string cmt_ACAG = std::string(global_ws_cmt->get_obj("ACAG"));

    EXPECT_EQ(global_ws_cmt->size(), 317);
    EXPECT_TRUE(global_ws_cmt->is_global_database());
    // global_ws_cmt->subset_instances contains the pointer to K_RWS[COMMENTS]
    EXPECT_EQ(global_ws_cmt->subset_instances.size(), 1);
    EXPECT_TRUE(global_ws_cmt->subset_instances.contains(K_RWS[COMMENTS][0]));

    std::set<std::string> names = global_ws_cmt->filter_names(pattern);

    std::set<std::string> expected_names;
    for (const std::string& name : names) 
        if (name.front() == 'A') 
            expected_names.insert(name);

    // ==== DEEP COPY SUBSET ====

    KDBComments* subset_deep_copy = new KDBComments(global_ws_cmt.get(), pattern, true);
    EXPECT_EQ(subset_deep_copy->size(), names.size());
    EXPECT_EQ(subset_deep_copy->get_names(), expected_names);
    EXPECT_TRUE(subset_deep_copy->is_local_database());
    EXPECT_TRUE(subset_deep_copy->db_parent == nullptr);
    EXPECT_TRUE(subset_deep_copy->subset_instances.empty());
    // global_ws_cmt->subset_instances contains the pointer to K_RWS[COMMENTS]
    EXPECT_EQ(global_ws_cmt->subset_instances.size(), 1);
    EXPECT_TRUE(global_ws_cmt->subset_instances.contains(K_RWS[COMMENTS][0]));

    // ---- add in deep copy subset only ----
    std::string new_comment = "new comment";
    subset_deep_copy->set_obj("NEW_COMMENT", new_comment);
    EXPECT_TRUE(subset_deep_copy->contains("NEW_COMMENT"));
    EXPECT_EQ(subset_deep_copy->get_obj("NEW_COMMENT"), new_comment);
    EXPECT_FALSE(global_ws_cmt->contains("NEW_COMMENT"));

    // ---- add in global KDB only ----
    new_comment = "new comment global";
    global_ws_cmt->set_obj("NEW_COMMENT_GLOBAL", new_comment);
    EXPECT_TRUE(global_ws_cmt->contains("NEW_COMMENT_GLOBAL"));
    EXPECT_EQ(global_ws_cmt->get_obj("NEW_COMMENT_GLOBAL"), new_comment);
    EXPECT_FALSE(subset_deep_copy->contains("NEW_COMMENT_GLOBAL"));

    // ---- modify in deep copy subset only ----
    subset_deep_copy->set_obj("ACAF", modified);
    EXPECT_EQ(global_ws_cmt->get_obj("ACAF"), cmt_ACAF);
    EXPECT_EQ(subset_deep_copy->get_obj("ACAF"), modified);

    // ---- modify in global KDB only ----
    global_ws_cmt->set_obj("ACAG", modified);
    EXPECT_EQ(global_ws_cmt->get_obj("ACAG"), modified);
    EXPECT_EQ(subset_deep_copy->get_obj("ACAG"), cmt_ACAG);

    // ---- rename in deep copy subset only ----
    subset_deep_copy->rename("ACAF", "ACAF_RENAMED");
    EXPECT_FALSE(subset_deep_copy->contains("ACAF"));
    EXPECT_TRUE(subset_deep_copy->contains("ACAF_RENAMED"));
    EXPECT_TRUE(global_ws_cmt->contains("ACAF"));
    EXPECT_FALSE(global_ws_cmt->contains("ACAF_RENAMED"));

    // ---- rename in global KDB only ----
    global_ws_cmt->rename("ACAG", "ACAG_RENAMED");
    EXPECT_FALSE(global_ws_cmt->contains("ACAG"));
    EXPECT_TRUE(global_ws_cmt->contains("ACAG_RENAMED"));

    // ---- remove in deep copy subset only ----
    subset_deep_copy->remove("AOUC");
    EXPECT_FALSE(subset_deep_copy->contains("AOUC"));
    EXPECT_TRUE(global_ws_cmt->contains("AOUC"));

    // ---- remove in global KDB only ----
    global_ws_cmt->remove("AQC");
    EXPECT_TRUE(subset_deep_copy->contains("AQC"));
    EXPECT_FALSE(global_ws_cmt->contains("AQC"));

    delete subset_deep_copy;

    // ==== SHALLOW COPY SUBSET ====

    global_ws_cmt->clear();
    global_ws_cmt->load(str_input_test_dir + "fun.ac");
    // global_ws_cmt->subset_instances contains the pointer to K_RWS[COMMENTS]
    EXPECT_EQ(global_ws_cmt->subset_instances.size(), 1);
    EXPECT_TRUE(global_ws_cmt->subset_instances.contains(K_RWS[COMMENTS][0]));

    KDBComments* subset_shallow_copy = new KDBComments(global_ws_cmt.get(), pattern, false);
    EXPECT_EQ(subset_shallow_copy->size(), names.size());
    EXPECT_EQ(subset_shallow_copy->get_names(), expected_names);
    EXPECT_TRUE(subset_shallow_copy->is_shallow_copy_database());
    EXPECT_TRUE(subset_shallow_copy->db_parent == global_ws_cmt.get());

    EXPECT_EQ(global_ws_cmt->subset_instances.size(), 2);
    EXPECT_TRUE(global_ws_cmt->subset_instances.contains(K_RWS[COMMENTS][0]));
    EXPECT_TRUE(global_ws_cmt->subset_instances.contains(subset_shallow_copy));

    // ---- add in shallow copy subset ----
    new_comment = "new comment";
    subset_shallow_copy->set_obj("NEW_COMMENT_2", new_comment);
    EXPECT_TRUE(global_ws_cmt->contains("NEW_COMMENT_2"));
    EXPECT_EQ(global_ws_cmt->get_obj("NEW_COMMENT_2"), new_comment);
    EXPECT_TRUE(subset_shallow_copy->contains("NEW_COMMENT_2"));
    EXPECT_EQ(subset_shallow_copy->get_obj("NEW_COMMENT_2"), new_comment);

    // ---- add in global KDB ----
    new_comment = "another new comment global";
    global_ws_cmt->set_obj("NEW_COMMENT_GLOBAL_2", new_comment);
    EXPECT_TRUE(global_ws_cmt->contains("NEW_COMMENT_GLOBAL_2"));
    EXPECT_EQ(global_ws_cmt->get_obj("NEW_COMMENT_GLOBAL_2"), new_comment);
    EXPECT_FALSE(subset_shallow_copy->contains("NEW_COMMENT_GLOBAL_2"));

    // ---- modify in shallow copy subset ----
    subset_shallow_copy->set_obj("ACAF", modified);
    EXPECT_EQ(global_ws_cmt->get_obj("ACAF"), modified);
    EXPECT_EQ(subset_shallow_copy->get_obj("ACAF"), modified);

    // ---- modify in global KDB ----
    global_ws_cmt->set_obj("ACAG", modified);
    EXPECT_EQ(global_ws_cmt->get_obj("ACAG"), modified);
    EXPECT_EQ(subset_shallow_copy->get_obj("ACAG"), modified);

    // ---- rename in shallow copy subset ----
    subset_shallow_copy->rename("ACAF", "ACAF_RENAMED_2");
    EXPECT_FALSE(subset_shallow_copy->contains("ACAF"));
    EXPECT_TRUE(subset_shallow_copy->contains("ACAF_RENAMED_2"));
    EXPECT_FALSE(global_ws_cmt->contains("ACAF"));
    EXPECT_TRUE(global_ws_cmt->contains("ACAF_RENAMED_2"));

    // ---- rename in global KDB ----
    global_ws_cmt->rename("ACAG", "ACAG_RENAMED_2");
    EXPECT_FALSE(global_ws_cmt->contains("ACAG"));
    EXPECT_TRUE(global_ws_cmt->contains("ACAG_RENAMED_2"));
    EXPECT_FALSE(subset_shallow_copy->contains("ACAG"));
    EXPECT_TRUE(subset_shallow_copy->contains("ACAG_RENAMED_2"));

    // ---- remove in shallow copy subset ----
    subset_shallow_copy->remove("AOUC");
    EXPECT_FALSE(subset_shallow_copy->contains("AOUC"));
    EXPECT_FALSE(global_ws_cmt->contains("AOUC"));

    // ---- remove in global KDB only ----
    global_ws_cmt->remove("AQC");
    EXPECT_FALSE(subset_shallow_copy->contains("AQC"));
    EXPECT_FALSE(global_ws_cmt->contains("AQC"));

    delete subset_shallow_copy;

    EXPECT_EQ(global_ws_cmt->subset_instances.size(), 1);
    EXPECT_TRUE(global_ws_cmt->subset_instances.contains(K_RWS[COMMENTS][0]));
}

TEST_F(SubsetsTest, MultiSubsets)
{
    std::string modified = "modified";
    std::string description = "Global comments KDB";

    global_ws_cmt->load(str_input_test_dir + "fun.ac");
    global_ws_cmt->description = description;
    std::string cmt_BENEF = std::string(global_ws_cmt->get_obj("BENEF"));

    // ==== subset of global KDB ====
    std::string pattern_0 = "A*;B*;*_";
    std::set<std::string> names_0 = global_ws_cmt->filter_names(pattern_0);
    std::set<std::string> expected_names_0;
    for (const std::string& name : global_ws_cmt->get_names()) 
        if (name.front() == 'A' || name.front() == 'B' || name.back() == '_')
            expected_names_0.insert(name);

    KDBComments* subset_0 = new KDBComments(global_ws_cmt.get(), pattern_0, false);
    EXPECT_EQ(subset_0->size(), names_0.size());
    EXPECT_EQ(subset_0->get_names(), expected_names_0);
    EXPECT_TRUE(subset_0->is_shallow_copy_database());
    EXPECT_TRUE(subset_0->db_parent == global_ws_cmt.get());
    EXPECT_EQ(subset_0->db_parent->description, description);
    // global_ws_cmt->subset_instances contains the pointer to K_RWS[COMMENTS]
    EXPECT_EQ(global_ws_cmt->subset_instances.size(), 2);
    EXPECT_TRUE(global_ws_cmt->subset_instances.contains(K_RWS[COMMENTS][0]));
    EXPECT_TRUE(global_ws_cmt->subset_instances.contains(subset_0));

    // ==== subset of subset ====
    std::string pattern_1 = "B*;*_";
    std::set<std::string> names_1 = subset_0->filter_names(pattern_1);
    std::set<std::string> expected_names_1;
    for(const auto& [name, handle] : subset_0->k_objs) 
        if(name.front() == 'B' || name.back() == '_')
            expected_names_1.insert(name);

    KDBComments* subset_1 = new KDBComments(subset_0, pattern_1, false);
    EXPECT_EQ(subset_1->size(), names_1.size());
    EXPECT_EQ(subset_1->get_names(), expected_names_1);
    EXPECT_TRUE(subset_1->is_shallow_copy_database());
    EXPECT_TRUE(subset_1->db_parent == global_ws_cmt.get());
    EXPECT_EQ(subset_1->db_parent->description, description);

    EXPECT_EQ(global_ws_cmt->subset_instances.size(), 3);
    EXPECT_TRUE(global_ws_cmt->subset_instances.contains(K_RWS[COMMENTS][0]));
    EXPECT_TRUE(global_ws_cmt->subset_instances.contains(subset_0));
    EXPECT_TRUE(global_ws_cmt->subset_instances.contains(subset_1));

    // ==== subset of subset of subset ====
    std::string pattern_2 = "B*;C*";
    std::set<std::string> names_2 = subset_1->filter_names(pattern_2);
    std::set<std::string> expected_names_2;
    for(const auto& [name, handle] : subset_1->k_objs) 
        if(name.front() == 'B' || name.front() == 'C')
            expected_names_2.insert(name);

    KDBComments* subset_2 = new KDBComments(subset_1, pattern_2, false);
    EXPECT_EQ(subset_2->size(), names_2.size());
    EXPECT_EQ(subset_2->get_names(), expected_names_2);
    EXPECT_TRUE(subset_2->is_shallow_copy_database());
    EXPECT_TRUE(subset_2->db_parent == global_ws_cmt.get());
    EXPECT_EQ(subset_2->db_parent->description, description);

    EXPECT_EQ(global_ws_cmt->subset_instances.size(), 4);
    EXPECT_TRUE(global_ws_cmt->subset_instances.contains(K_RWS[COMMENTS][0]));
    EXPECT_TRUE(global_ws_cmt->subset_instances.contains(subset_0));
    EXPECT_TRUE(global_ws_cmt->subset_instances.contains(subset_1));
    EXPECT_TRUE(global_ws_cmt->subset_instances.contains(subset_2));

    // ==== add, modify, rename, remove ====
    // ---- add in subset ----
    std::string new_comment = "new comment";
    subset_1->set_obj("BENEF_NEW", new_comment);
    EXPECT_TRUE(global_ws_cmt->contains("BENEF_NEW"));
    EXPECT_EQ(global_ws_cmt->get_obj("BENEF_NEW"), new_comment);
    EXPECT_TRUE(subset_1->contains("BENEF_NEW"));
    EXPECT_EQ(subset_1->get_obj("BENEF_NEW"), new_comment);
    EXPECT_FALSE(subset_0->contains("BENEF_NEW"));
    EXPECT_FALSE(subset_2->contains("BENEF_NEW"));

    // ---- add in global KDB ----
    new_comment = "new comment global";
    global_ws_cmt->set_obj("BQY_NEW", new_comment);
    EXPECT_TRUE(global_ws_cmt->contains("BQY_NEW"));
    EXPECT_EQ(global_ws_cmt->get_obj("BQY_NEW"), new_comment);
    EXPECT_FALSE(subset_0->contains("BQY_NEW"));
    EXPECT_FALSE(subset_1->contains("BQY_NEW"));
    EXPECT_FALSE(subset_2->contains("BQY_NEW"));

    // ---- modify in subset ----
    subset_1->set_obj("BENEF", modified);
    EXPECT_EQ(global_ws_cmt->get_obj("BENEF"), modified);
    EXPECT_EQ(subset_0->get_obj("BENEF"), modified);
    EXPECT_EQ(subset_1->get_obj("BENEF"), modified);
    EXPECT_EQ(subset_2->get_obj("BENEF"), modified);

    // ---- modify in global KDB ----
    global_ws_cmt->set_obj("BQY", modified);
    EXPECT_EQ(global_ws_cmt->get_obj("BQY"), modified);
    EXPECT_EQ(subset_0->get_obj("BQY"), modified);
    EXPECT_EQ(subset_1->get_obj("BQY"), modified);
    EXPECT_EQ(subset_2->get_obj("BQY"), modified);

    // ---- rename in subset ----
    subset_1->rename("BENEF", "BENEF_RENAMED");
    EXPECT_FALSE(global_ws_cmt->contains("BENEF"));
    EXPECT_TRUE(global_ws_cmt->contains("BENEF_RENAMED"));
    EXPECT_FALSE(subset_0->contains("BENEF"));
    EXPECT_TRUE(subset_0->contains("BENEF_RENAMED"));
    EXPECT_FALSE(subset_1->contains("BENEF"));
    EXPECT_TRUE(subset_1->contains("BENEF_RENAMED"));
    EXPECT_FALSE(subset_2->contains("BENEF"));
    EXPECT_TRUE(subset_2->contains("BENEF_RENAMED"));

    // ---- rename in global KDB ----
    global_ws_cmt->rename("BQY", "BQY_RENAMED");
    EXPECT_FALSE(global_ws_cmt->contains("BQY"));
    EXPECT_TRUE(global_ws_cmt->contains("BQY_RENAMED"));
    EXPECT_FALSE(subset_0->contains("BQY"));
    EXPECT_TRUE(subset_0->contains("BQY_RENAMED"));
    EXPECT_FALSE(subset_1->contains("BQY"));
    EXPECT_TRUE(subset_1->contains("BQY_RENAMED"));
    EXPECT_FALSE(subset_2->contains("BQY"));
    EXPECT_TRUE(subset_2->contains("BQY_RENAMED"));

    // ---- remove in subset ----
    subset_1->remove("BENEF_");
    EXPECT_FALSE(global_ws_cmt->contains("BENEF_"));
    EXPECT_FALSE(subset_0->contains("BENEF_"));
    EXPECT_FALSE(subset_1->contains("BENEF_"));
    EXPECT_FALSE(subset_2->contains("BENEF_"));

    // ---- remove in global KDB ----
    subset_1->remove("BVY");
    EXPECT_FALSE(global_ws_cmt->contains("BVY"));
    EXPECT_FALSE(subset_0->contains("BVY"));
    EXPECT_FALSE(subset_1->contains("BVY"));
    EXPECT_FALSE(subset_2->contains("BVY"));

    delete subset_2;
    delete subset_1;
    delete subset_0;

    EXPECT_EQ(global_ws_cmt->subset_instances.size(), 1);
    EXPECT_TRUE(global_ws_cmt->subset_instances.contains(K_RWS[COMMENTS][0]));
}
