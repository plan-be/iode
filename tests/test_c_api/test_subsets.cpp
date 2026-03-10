#include "pch.h"

class SubsetsTest : public TestAbstract, public ::testing::Test 
{
protected:
    void SetUp() override 
    {
        global_ws_cmt->clear();
        if(global_ref_cmt[0])
        {
            delete global_ref_cmt[0];
            global_ref_cmt[0] = nullptr;
        }
    }

    void TearDown() override 
    {
        global_ws_cmt->clear();
        if(global_ref_cmt[0])
        {
            delete global_ref_cmt[0];
            global_ref_cmt[0] = nullptr;
        }
    }
};


TEST_F(SubsetsTest, Subset)
{
    KDBComments* ref_db_cmt = nullptr;
    std::string pattern = "A*";
    std::string modified = "modified";

    // GLOBAL KDB
    global_ws_cmt->load(str_input_test_dir + "fun.ac");
    ref_db_cmt = (KDBComments*) global_ref_cmt[0];      // updated in the load() method
    std::string cmt_ACAF = global_ws_cmt->get("ACAF");
    std::string cmt_ACAG = global_ws_cmt->get("ACAG");

    int expected_size = 317;
    EXPECT_EQ(global_ws_cmt->size(), expected_size);
    EXPECT_TRUE(global_ws_cmt->is_global_database());
    // global_ws_cmt->children_db contains the pointer to global_ref_cmt
    EXPECT_EQ(global_ws_cmt->children_db.size(), 1);
    EXPECT_TRUE(global_ws_cmt->children_db.contains(ref_db_cmt));

    std::set<std::string> names = global_ws_cmt->filter_names(pattern);

    std::set<std::string> expected_names;
    for (const std::string& name : names) 
        if (name.front() == 'A') 
            expected_names.insert(name);

    // ==== STANDALONE KDB -> DEEP COPY SUBSET ====

    KDBComments* standalone_db = new KDBComments(global_ws_cmt.get(), pattern, true);
    EXPECT_EQ(standalone_db->size(), names.size());
    EXPECT_EQ(standalone_db->get_names(), expected_names);
    EXPECT_TRUE(standalone_db->is_detached_database());
    EXPECT_TRUE(standalone_db->db_parent == nullptr);
    EXPECT_TRUE(standalone_db->children_db.empty());
    // global_ws_cmt->children_db contains the pointer to global_ref_cmt
    EXPECT_EQ(global_ws_cmt->children_db.size(), 1);
    EXPECT_TRUE(global_ws_cmt->children_db.contains(ref_db_cmt));

    // ---- add in standalone database only ----
    // the new comment is added in the standalone database only and 
    // not in the global KDB since it is a deep copy subset of the global KDB
    std::string new_comment = "new comment";
    standalone_db->add("NEW_COMMENT", new_comment);
    EXPECT_TRUE(standalone_db->contains("NEW_COMMENT"));
    EXPECT_EQ(standalone_db->get("NEW_COMMENT"), new_comment);
    EXPECT_FALSE(global_ws_cmt->contains("NEW_COMMENT"));

    // ---- add in global KDB only ----
    // the new comment is added in the global KDB only and not in the standalone database 
    // since it is a deep copy subset of the global KDB
    new_comment = "new comment global";
    global_ws_cmt->add("NEW_COMMENT_GLOBAL", new_comment);
    EXPECT_TRUE(global_ws_cmt->contains("NEW_COMMENT_GLOBAL"));
    EXPECT_EQ(global_ws_cmt->get("NEW_COMMENT_GLOBAL"), new_comment);
    EXPECT_FALSE(standalone_db->contains("NEW_COMMENT_GLOBAL"));

    // ---- modify in standalone database only ----
    // the comment is modified in the standalone database only and not in the 
    // global KDB since it is a deep copy subset of the global KDB
    standalone_db->update("ACAF", modified);
    EXPECT_EQ(global_ws_cmt->get("ACAF"), cmt_ACAF);
    EXPECT_EQ(standalone_db->get("ACAF"), modified);

    // ---- modify in global KDB only ----
    // the comment is modified in the global KDB only and not in the standalone database 
    // since it is a deep copy subset of the global KDB
    global_ws_cmt->update("ACAG", modified);
    EXPECT_EQ(global_ws_cmt->get("ACAG"), modified);
    EXPECT_EQ(standalone_db->get("ACAG"), cmt_ACAG);

    // ---- rename in standalone database only ----
    // the comment is renamed in the standalone database only and not in the global KDB 
    // since it is a deep copy subset of the global KDB
    standalone_db->rename("ACAF", "ACAF_RENAMED");
    EXPECT_FALSE(standalone_db->contains("ACAF"));
    EXPECT_TRUE(standalone_db->contains("ACAF_RENAMED"));
    EXPECT_TRUE(global_ws_cmt->contains("ACAF"));
    EXPECT_FALSE(global_ws_cmt->contains("ACAF_RENAMED"));

    // ---- rename in global KDB only ----
    // the comment is renamed in the global KDB only and not in the standalone database
    // since it is a deep copy subset of the global KDB
    global_ws_cmt->rename("ACAG", "ACAG_RENAMED");
    EXPECT_FALSE(global_ws_cmt->contains("ACAG"));
    EXPECT_TRUE(global_ws_cmt->contains("ACAG_RENAMED"));

    // ---- remove in standalone database only ----
    // the comment is removed in the standalone database only and not in the global KDB
    // since it is a deep copy subset of the global KDB
    standalone_db->remove("AOUC");
    EXPECT_FALSE(standalone_db->contains("AOUC"));
    EXPECT_TRUE(global_ws_cmt->contains("AOUC"));

    // ---- remove in global KDB only ----
    // the comment is removed in the global KDB only and not in the standalone database
    // since it is a deep copy subset of the global KDB
    global_ws_cmt->remove("AQC");
    EXPECT_TRUE(standalone_db->contains("AQC"));
    EXPECT_FALSE(global_ws_cmt->contains("AQC"));

    delete standalone_db;
    // after deleting the standalone database, the global KDB should remain unchanged 
    // and the pointer to global_ref_cmt should still be in global_ws_cmt->children_db
    EXPECT_EQ(global_ws_cmt->size(), expected_size);
    EXPECT_TRUE(global_ws_cmt->is_global_database());
    EXPECT_EQ(global_ws_cmt->children_db.size(), 1);
    EXPECT_TRUE(global_ws_cmt->children_db.contains(ref_db_cmt));

    // ==== SHALLOW COPY SUBSET ====

    global_ws_cmt->clear();
    global_ws_cmt->load(str_input_test_dir + "fun.ac");
    ref_db_cmt = (KDBComments*) global_ref_cmt[0];      // updated in the load() method
    // global_ws_cmt->children_db contains the pointer to global_ref_cmt
    EXPECT_EQ(global_ws_cmt->children_db.size(), 1);
    EXPECT_TRUE(global_ws_cmt->children_db.contains(ref_db_cmt));

    KDBComments* subset_shallow_copy = new KDBComments(global_ws_cmt.get(), pattern, false);
    EXPECT_EQ(subset_shallow_copy->size(), names.size());
    EXPECT_EQ(subset_shallow_copy->get_names(), expected_names);
    EXPECT_TRUE(subset_shallow_copy->is_subset_database());
    EXPECT_TRUE(subset_shallow_copy->db_parent == global_ws_cmt.get());

    EXPECT_EQ(global_ws_cmt->children_db.size(), 2);
    EXPECT_TRUE(global_ws_cmt->children_db.contains(ref_db_cmt));
    EXPECT_TRUE(global_ws_cmt->children_db.contains(subset_shallow_copy));

    // ---- add in shallow copy subset ----
    // the new comment is added in the shallow copy subset AND in the global KDB
    new_comment = "new comment";
    subset_shallow_copy->add("NEW_COMMENT_2", new_comment);
    EXPECT_TRUE(global_ws_cmt->contains("NEW_COMMENT_2"));
    EXPECT_EQ(global_ws_cmt->get("NEW_COMMENT_2"), new_comment);
    EXPECT_TRUE(subset_shallow_copy->contains("NEW_COMMENT_2"));
    EXPECT_EQ(subset_shallow_copy->get("NEW_COMMENT_2"), new_comment);

    // ---- add in global KDB ----
    // the new comment is added in the global KDB only and NOT in the shallow copy subset
    // NOTE: by design, the add() method of the global KDB does not propagate the new comment 
    //       to its subsets
    new_comment = "another new comment global";
    global_ws_cmt->add("NEW_COMMENT_GLOBAL_2", new_comment);
    EXPECT_TRUE(global_ws_cmt->contains("NEW_COMMENT_GLOBAL_2"));
    EXPECT_EQ(global_ws_cmt->get("NEW_COMMENT_GLOBAL_2"), new_comment);
    EXPECT_FALSE(subset_shallow_copy->contains("NEW_COMMENT_GLOBAL_2"));

    // ---- modify in shallow copy subset ----
    // the comment is modified in the shallow copy subset AND in the global KDB 
    // since it is a shallow copy subset of the global KDB (same pointer)
    subset_shallow_copy->update("ACAF", modified);
    EXPECT_EQ(global_ws_cmt->get("ACAF"), modified);
    EXPECT_EQ(subset_shallow_copy->get("ACAF"), modified);

    // ---- modify in global KDB ----
    // the comment is modified in the global KDB AND in the shallow copy subset
    // since it is a shallow copy subset of the global KDB (same pointer)
    global_ws_cmt->update("ACAG", modified);
    EXPECT_EQ(global_ws_cmt->get("ACAG"), modified);
    EXPECT_EQ(subset_shallow_copy->get("ACAG"), modified);

    // ---- rename in shallow copy subset ----
    // the comment is renamed in the shallow copy subset AND in the global KDB
    // NOTE: by design, the rename() method of the shallow copy subset propagates 
    //       the change to the global KDB but also to all other of its subsets (if any)
    subset_shallow_copy->rename("ACAF", "ACAF_RENAMED_2");
    EXPECT_FALSE(subset_shallow_copy->contains("ACAF"));
    EXPECT_TRUE(subset_shallow_copy->contains("ACAF_RENAMED_2"));
    EXPECT_FALSE(global_ws_cmt->contains("ACAF"));
    EXPECT_TRUE(global_ws_cmt->contains("ACAF_RENAMED_2"));

    // ---- rename in global KDB ----
    // the comment is renamed in the global KDB AND in the shallow copy subset
    // NOTE: by design, the rename() method of the global KDB propagates 
    //       the change to all of its subsets (if any)
    global_ws_cmt->rename("ACAG", "ACAG_RENAMED_2");
    EXPECT_FALSE(global_ws_cmt->contains("ACAG"));
    EXPECT_TRUE(global_ws_cmt->contains("ACAG_RENAMED_2"));
    EXPECT_FALSE(subset_shallow_copy->contains("ACAG"));
    EXPECT_TRUE(subset_shallow_copy->contains("ACAG_RENAMED_2"));

    // ---- remove in shallow copy subset ----
    // the comment is removed in the shallow copy subset AND in the global KDB
    // NOTE: by design, the remove() method of the shallow copy subset propagates
    //       the change to the global KDB but also to all other of its subsets (if any)
    subset_shallow_copy->remove("AOUC");
    EXPECT_FALSE(subset_shallow_copy->contains("AOUC"));
    EXPECT_FALSE(global_ws_cmt->contains("AOUC"));

    // ---- remove in global KDB only ----
    // the comment is removed in the global KDB AND in the shallow copy subset
    // NOTE: by design, the remove() method of the global KDB propagates
    //       the change to all of its subsets (if any)
    global_ws_cmt->remove("AQC");
    EXPECT_FALSE(subset_shallow_copy->contains("AQC"));
    EXPECT_FALSE(global_ws_cmt->contains("AQC"));

    delete subset_shallow_copy;
    // deleting the subset only reset the k_objs map of the subset but DO NOT DELETE 
    // the objects. Therefore, the global KDB should remain unchanged and the pointer 
    // to global_ref_cmt should still be in global_ws_cmt->children_db
    EXPECT_EQ(global_ws_cmt->size(), expected_size);
    EXPECT_TRUE(global_ws_cmt->is_global_database());
    EXPECT_EQ(global_ws_cmt->children_db.size(), 1);
    EXPECT_TRUE(global_ws_cmt->children_db.contains(ref_db_cmt));
}

TEST_F(SubsetsTest, MultiSubsets)
{
    KDBComments* ref_db_cmt = nullptr;
    std::string modified = "modified";
    std::string description = "Global comments KDB";

    global_ws_cmt->load(str_input_test_dir + "fun.ac");
    ref_db_cmt = (KDBComments*) global_ref_cmt[0];      // updated in the load() method
    global_ws_cmt->description = description;
    std::string cmt_BENEF = global_ws_cmt->get("BENEF");

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
    EXPECT_TRUE(subset_0->is_subset_database());
    EXPECT_TRUE(subset_0->db_parent == global_ws_cmt.get());
    EXPECT_EQ(subset_0->db_parent->description, description);
    // global_ws_cmt->children_db contains the pointer to global_ref_cmt
    EXPECT_EQ(global_ws_cmt->children_db.size(), 2);
    EXPECT_TRUE(global_ws_cmt->children_db.contains(ref_db_cmt));
    EXPECT_TRUE(global_ws_cmt->children_db.contains(subset_0));

    // ==== subset of subset ====
    std::string pattern_1 = "B*;*_";
    std::set<std::string> names_1 = subset_0->filter_names(pattern_1);
    std::set<std::string> expected_names_1;
    for(const auto& [name, _] : subset_0->k_objs) 
        if(name.front() == 'B' || name.back() == '_')
            expected_names_1.insert(name);

    KDBComments* subset_1 = new KDBComments(subset_0, pattern_1, false);
    EXPECT_EQ(subset_1->size(), names_1.size());
    EXPECT_EQ(subset_1->get_names(), expected_names_1);
    EXPECT_TRUE(subset_1->is_subset_database());
    EXPECT_TRUE(subset_1->db_parent == global_ws_cmt.get());
    EXPECT_EQ(subset_1->db_parent->description, description);

    EXPECT_EQ(global_ws_cmt->children_db.size(), 3);
    EXPECT_TRUE(global_ws_cmt->children_db.contains(ref_db_cmt));
    EXPECT_TRUE(global_ws_cmt->children_db.contains(subset_0));
    EXPECT_TRUE(global_ws_cmt->children_db.contains(subset_1));

    // ==== subset of subset of subset ====
    std::string pattern_2 = "B*;C*";
    std::set<std::string> names_2 = subset_1->filter_names(pattern_2);
    std::set<std::string> expected_names_2;
    for(const auto& [name, _] : subset_1->k_objs) 
        if(name.front() == 'B' || name.front() == 'C')
            expected_names_2.insert(name);

    KDBComments* subset_2 = new KDBComments(subset_1, pattern_2, false);
    EXPECT_EQ(subset_2->size(), names_2.size());
    EXPECT_EQ(subset_2->get_names(), expected_names_2);
    EXPECT_TRUE(subset_2->is_subset_database());
    EXPECT_TRUE(subset_2->db_parent == global_ws_cmt.get());
    EXPECT_EQ(subset_2->db_parent->description, description);

    EXPECT_EQ(global_ws_cmt->children_db.size(), 4);
    EXPECT_TRUE(global_ws_cmt->children_db.contains(ref_db_cmt));
    EXPECT_TRUE(global_ws_cmt->children_db.contains(subset_0));
    EXPECT_TRUE(global_ws_cmt->children_db.contains(subset_1));
    EXPECT_TRUE(global_ws_cmt->children_db.contains(subset_2));

    // ==== add, modify, rename, remove ====
    // ---- add in subset ----
    std::string new_comment = "new comment";
    subset_1->add("BENEF_NEW", new_comment);
    EXPECT_TRUE(global_ws_cmt->contains("BENEF_NEW"));
    EXPECT_EQ(global_ws_cmt->get("BENEF_NEW"), new_comment);
    EXPECT_TRUE(subset_1->contains("BENEF_NEW"));
    EXPECT_EQ(subset_1->get("BENEF_NEW"), new_comment);
    EXPECT_FALSE(subset_0->contains("BENEF_NEW"));
    EXPECT_FALSE(subset_2->contains("BENEF_NEW"));

    // ---- add in global KDB ----
    new_comment = "new comment global";
    global_ws_cmt->add("BQY_NEW", new_comment);
    EXPECT_TRUE(global_ws_cmt->contains("BQY_NEW"));
    EXPECT_EQ(global_ws_cmt->get("BQY_NEW"), new_comment);
    EXPECT_FALSE(subset_0->contains("BQY_NEW"));
    EXPECT_FALSE(subset_1->contains("BQY_NEW"));
    EXPECT_FALSE(subset_2->contains("BQY_NEW"));

    // ---- modify in subset ----
    subset_1->update("BENEF", modified);
    EXPECT_EQ(global_ws_cmt->get("BENEF"), modified);
    EXPECT_EQ(subset_0->get("BENEF"), modified);
    EXPECT_EQ(subset_1->get("BENEF"), modified);
    EXPECT_EQ(subset_2->get("BENEF"), modified);

    // ---- modify in global KDB ----
    global_ws_cmt->update("BQY", modified);
    EXPECT_EQ(global_ws_cmt->get("BQY"), modified);
    EXPECT_EQ(subset_0->get("BQY"), modified);
    EXPECT_EQ(subset_1->get("BQY"), modified);
    EXPECT_EQ(subset_2->get("BQY"), modified);

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

    EXPECT_EQ(global_ws_cmt->children_db.size(), 1);
    EXPECT_TRUE(global_ws_cmt->children_db.contains(ref_db_cmt));
}

TEST_F(SubsetsTest, AddDeletePtr)
{
    double value = 0.5;
    double relax = 0.8;
    double std = IODE_NAN;
    
    std::shared_ptr<Scalar> scl_ptr;
    std::shared_ptr<Scalar> scl_ptr_2;
    std::shared_ptr<Scalar> scl_ptr_one;
    std::shared_ptr<Scalar> scl_ptr_bis;
    std::shared_ptr<Scalar> scl_ptr_ter;
    std::shared_ptr<Scalar> scl_global_ws;
    std::shared_ptr<Scalar> scl_subset;
    std::shared_ptr<Scalar> scl_standalone_db;
    bool copy;

    // ==== add scalar in global KDB ====
    scl_ptr = std::make_shared<Scalar>(value, relax);
    scl_ptr_one = global_ws_scl->set_obj_ptr("scl_global", scl_ptr);
    global_ws_scl->set_obj_ptr("scl_global_bis", scl_ptr);
    global_ws_scl->set_obj_ptr("scl_global_ter", scl_ptr);
    EXPECT_TRUE(global_ws_scl->contains("scl_global"));
    scl_global_ws = global_ws_scl->get_obj_ptr("scl_global");
    EXPECT_DOUBLE_EQ(scl_global_ws->value, scl_ptr->value);
    EXPECT_DOUBLE_EQ(scl_global_ws->relax, scl_ptr->relax);
    EXPECT_DOUBLE_EQ(scl_global_ws->std, scl_ptr->std);
    // check that the same pointer is used by scl_ptr and the global KDB
    EXPECT_TRUE(scl_global_ws == scl_ptr_one);
    // check that a different pointer has been created in the global KDB 
    // for the three different names
    scl_ptr_bis = global_ws_scl->get_obj_ptr("scl_global_bis");
    scl_ptr_ter = global_ws_scl->get_obj_ptr("scl_global_ter");
    EXPECT_TRUE(scl_ptr_bis != scl_ptr_one);
    EXPECT_TRUE(scl_ptr_ter != scl_ptr_bis);
    // check that resetting the pointer scl_ptr does not reset the pointer 
    // in the global KDB since the set_obj_ptr() method
    scl_ptr.reset();
    EXPECT_TRUE(scl_ptr.get() == nullptr);
    EXPECT_TRUE(scl_global_ws.get() != nullptr);

    // ==== subset of global KDB ====
    copy = false;    // shallow copy
    KDBScalars* subset = new KDBScalars(global_ws_scl.get(), "s*", copy);
    scl_ptr = std::make_shared<Scalar>(value, relax);
    scl_ptr_one = subset->set_obj_ptr("scl_subset", scl_ptr);
    scl_ptr_bis = subset->set_obj_ptr("scl_subset_bis", scl_ptr);
    scl_ptr_ter = subset->set_obj_ptr("scl_subset_ter", scl_ptr);
    EXPECT_TRUE(subset->contains("scl_subset"));
    EXPECT_TRUE(global_ws_scl->contains("scl_subset"));
    scl_subset = subset->get_obj_ptr("scl_subset");
    EXPECT_DOUBLE_EQ(scl_subset->value, scl_ptr->value);
    EXPECT_DOUBLE_EQ(scl_subset->relax, scl_ptr->relax);
    EXPECT_DOUBLE_EQ(scl_subset->std, scl_ptr->std);
    // check that the same pointer is used in the global KDB and 
    // in the subset (shallow copy)
    scl_global_ws = global_ws_scl->get_obj_ptr("scl_subset");
    EXPECT_TRUE(scl_subset == scl_ptr_one);
    EXPECT_TRUE(scl_subset == scl_global_ws);
    // check that a different pointer has been created in the subset KDB 
    // for the three different names 
    EXPECT_TRUE(scl_ptr_bis != scl_ptr_one);
    EXPECT_TRUE(scl_ptr_ter != scl_ptr_bis);    
    // check that resetting the pointer scl_ptr does not reset 
    // the pointer in the global KDB and in the subset
    scl_ptr.reset();
    EXPECT_TRUE(scl_ptr.get() == nullptr);
    EXPECT_TRUE(scl_subset.get() != nullptr);
    EXPECT_TRUE(scl_global_ws.get() != nullptr);

    // ==== standalone KDB (deep copy) ====
    scl_ptr_2 = std::make_shared<Scalar>(value, relax);
    subset->set_obj_ptr("scl_common", scl_ptr_2);
    copy = true;    // deep copy
    KDBScalars* standalone_db = new KDBScalars(global_ws_scl.get(), "s*", copy);
    EXPECT_TRUE(standalone_db->contains("scl_common"));
    EXPECT_TRUE(global_ws_scl->contains("scl_common"));
    EXPECT_TRUE(subset->contains("scl_common"));
    // check that the pointer in the standalone_db is different from the pointer 
    // in the global KDB and in the subset (deep copy)
    scl_standalone_db = standalone_db->get_obj_ptr("scl_common");
    scl_global_ws = global_ws_scl->get_obj_ptr("scl_common");
    scl_subset = subset->get_obj_ptr("scl_common");
    EXPECT_TRUE(scl_standalone_db != scl_ptr_2);
    EXPECT_TRUE(scl_global_ws != scl_standalone_db);
    EXPECT_TRUE(scl_subset != scl_standalone_db);

    // adding a new object in the standalone_db should not affect the global KDB 
    // and the subset since the standalone_db is standalone
    scl_ptr_2 = std::make_shared<Scalar>(value, relax);
    scl_ptr_one = standalone_db->set_obj_ptr("scl_standalone", scl_ptr_2);
    scl_ptr_bis = standalone_db->set_obj_ptr("scl_standalone_bis", scl_ptr_2);
    scl_ptr_ter = standalone_db->set_obj_ptr("scl_standalone_ter", scl_ptr_2);
    EXPECT_TRUE(standalone_db->contains("scl_standalone"));
    EXPECT_FALSE(global_ws_scl->contains("scl_standalone"));
    EXPECT_FALSE(subset->contains("scl_standalone"));
    scl_standalone_db = standalone_db->get_obj_ptr("scl_standalone");
    EXPECT_TRUE(scl_standalone_db == scl_ptr_one);
    EXPECT_DOUBLE_EQ(scl_standalone_db->value, scl_ptr_2->value);
    EXPECT_DOUBLE_EQ(scl_standalone_db->relax, scl_ptr_2->relax);
    EXPECT_DOUBLE_EQ(scl_standalone_db->std, scl_ptr_2->std);
    // check that a different pointer has been created in the standalone KDB 
    // for the three different names 
    EXPECT_TRUE(scl_ptr_bis != scl_ptr_one);
    EXPECT_TRUE(scl_ptr_ter != scl_ptr_bis); 

    // ==== delete subset ====
    scl_subset = subset->get_obj_ptr("scl_subset");
    delete subset;
    // deleting the subset only clear the k_objs map of the subset but
    // DO NOT RESET the associated shared pointers
    EXPECT_TRUE(scl_subset.get() != nullptr);
    EXPECT_DOUBLE_EQ(scl_subset->value, value);
    EXPECT_DOUBLE_EQ(scl_subset->relax, relax);
    EXPECT_DOUBLE_EQ(scl_subset->std, std);
    // deleting the subset only clear the k_objs map of the subset but
    // DO NOT RESET the associated shared pointers. 
    // Therefore, the global KDB should remain unchanged
    EXPECT_TRUE(global_ws_scl->contains("scl_subset"));
    scl_global_ws = global_ws_scl->get_obj_ptr("scl_subset");
    EXPECT_DOUBLE_EQ(scl_global_ws->value, value);
    EXPECT_DOUBLE_EQ(scl_global_ws->relax, relax);
    EXPECT_DOUBLE_EQ(scl_global_ws->std, std);
    EXPECT_TRUE(global_ws_scl->contains("scl_global"));
    scl_global_ws = global_ws_scl->get_obj_ptr("scl_global");
    EXPECT_DOUBLE_EQ(scl_global_ws->value, value);
    EXPECT_DOUBLE_EQ(scl_global_ws->relax, relax);
    EXPECT_DOUBLE_EQ(scl_global_ws->std, std);
    // deleting the subset should not delete the objects in the standalone_db 
    // since standalone_db represents a deep copy of the global KDB
    EXPECT_TRUE(standalone_db->contains("scl_standalone"));
    scl_standalone_db = standalone_db->get_obj_ptr("scl_standalone");
    EXPECT_DOUBLE_EQ(scl_standalone_db->value, value);
    EXPECT_DOUBLE_EQ(scl_standalone_db->relax, relax);
    EXPECT_DOUBLE_EQ(scl_standalone_db->std, std);

    // ==== delete global KDB ====
    global_ws_scl->clear();
    EXPECT_EQ(global_ws_scl->size(), 0);
    // clearing the global KDB should not affect the standalone_db 
    // since standalone_db represents a deep copy of the global KDB
    EXPECT_TRUE(standalone_db->contains("scl_common"));
    scl_standalone_db = standalone_db->get_obj_ptr("scl_common");
    EXPECT_DOUBLE_EQ(scl_standalone_db->value, value);
    EXPECT_DOUBLE_EQ(scl_standalone_db->relax, relax);
    EXPECT_DOUBLE_EQ(scl_standalone_db->std, std);

    // ==== delete standalone_db ==== 
    delete standalone_db;

    // **** changing order of deletion ****

    scl_ptr.reset();
    scl_ptr_2.reset();
    scl_global_ws.reset();
    scl_subset.reset();
    scl_standalone_db.reset();

    scl_ptr = std::make_shared<Scalar>(value, relax);
    global_ws_scl->set_obj_ptr("scl_global", scl_ptr);
    EXPECT_TRUE(global_ws_scl->contains("scl_global"));

    copy = false;    // shallow copy
    subset = new KDBScalars(global_ws_scl.get(), "s*", copy);
    scl_ptr = std::make_shared<Scalar>(value, relax);
    subset->set_obj_ptr("scl_subset", scl_ptr);
    EXPECT_TRUE(subset->contains("scl_subset"));

    copy = true;    // deep copy
    standalone_db = new KDBScalars(global_ws_scl.get(), "s*", copy);
    scl_ptr_2 = std::make_shared<Scalar>(value, relax);
    standalone_db->set_obj_ptr("scl_standalone", scl_ptr_2);
    EXPECT_TRUE(standalone_db->contains("scl_standalone"));
    
    // ==== delete the standalone_db first ====
    delete standalone_db;
    // deleting the standalone_db should not affect the global KDB and the subset 
    // since standalone_db represents a deep copy of the global KDB
    EXPECT_TRUE(global_ws_scl->contains("scl_global"));
    scl_global_ws = global_ws_scl->get_obj_ptr("scl_global");
    EXPECT_DOUBLE_EQ(scl_global_ws->value, value);
    EXPECT_DOUBLE_EQ(scl_global_ws->relax, relax);
    EXPECT_DOUBLE_EQ(scl_global_ws->std, std);
    EXPECT_TRUE(subset->contains("scl_subset"));
    scl_subset = subset->get_obj_ptr("scl_subset");
    EXPECT_DOUBLE_EQ(scl_subset->value, value);
    EXPECT_DOUBLE_EQ(scl_subset->relax, relax);
    EXPECT_DOUBLE_EQ(scl_subset->std, std);
    // deleting the standalone_db should not delete the original pointer scl
    // since the set_obj_ptr() method creates a copy of the original pointer
    EXPECT_TRUE(scl_ptr.get() != nullptr);
    EXPECT_DOUBLE_EQ(scl_ptr->value, value);
    EXPECT_DOUBLE_EQ(scl_ptr->relax, relax);
    EXPECT_DOUBLE_EQ(scl_ptr->std, std);

    // ==== delete the global KDB before the subset ====
    global_ws_scl->clear();
    EXPECT_EQ(global_ws_scl->size(), 0);
    EXPECT_FALSE(global_ws_scl->contains("scl_global"));
    // deleting the global KDB should clear all the objects in the global KDB 
    // AND in its subsets too
    EXPECT_EQ(subset->size(), 0);
    EXPECT_FALSE(subset->contains("scl_subset"));

    // deleting the global KDB should not reset the original pointer scl 
    // since deleting or clearing the global KDB only clear the k_objs map 
    // of the global KDB but DO NOT RESET the shared pointers.
    EXPECT_TRUE(scl_ptr.get() != nullptr);
    EXPECT_DOUBLE_EQ(scl_ptr->value, value);
    EXPECT_DOUBLE_EQ(scl_ptr->relax, relax);
    EXPECT_DOUBLE_EQ(scl_ptr->std, std);
}
