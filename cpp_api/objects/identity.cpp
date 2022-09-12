#include "identity.h"

static IDT* extract_identity(KDB* kdb, const int pos)
{
    return K_iunpack(SW_getptr(kdb->k_objs[pos].o_val));
}


Identity::Identity()
{
    c_identity = nullptr;
}

Identity::Identity(const int pos, KDB* kdb)
{
    if (!kdb) kdb = K_WS[I_SCALARS];
    if (pos < 0 || pos > kdb->k_nb)
    {
        IodeExceptionInvalidArguments error("Cannot extract Scalar", "Scalar position must be in range [0, " + 
            std::to_string(kdb->k_nb - 1) + "])");
        error.add_argument("equation position", std::to_string(pos));
        throw error;
    }
    c_identity = extract_identity(kdb, pos);
}

Identity::Identity(const std::string& name, KDB* kdb)
{
    if (!kdb) kdb = K_WS[I_SCALARS];
    int pos = K_find(kdb, to_char_array(name));
    if (pos < 0) throw IodeExceptionFunction("Cannot extract Scalar", "Scalar with name " + name + " does not exist.");
    c_identity = extract_identity(kdb, pos);
}

Identity::Identity(const std::string& lec)
{
    // QUESTION FOR JMP: Should I allocate c_identity->clec by default ?
    c_identity = (IDT*) SW_nalloc(sizeof(IDT));
    c_identity->lec = copy_string_to_char(lec);
    c_identity->clec = L_cc(to_char_array(lec));
}

Identity::Identity(const Identity& idt)
{
    c_identity = (IDT*) SW_nalloc(sizeof(IDT));
    c_identity->lec = (char*) SCR_stracpy((unsigned char*) idt.c_identity->lec);
    c_identity->clec = (CLEC*) SW_nalloc(sizeof(CLEC));
    memcpy(c_identity->clec, idt.c_identity->clec, sizeof(CLEC));
}

Identity::~Identity()
{
    if(c_identity)
    {
        SW_nfree(c_identity->lec);
        SW_nfree(c_identity->clec);
        SW_nfree(c_identity);
    }
}

// required to be used in std::map
Identity& Identity::operator=(const Identity& idt)
{
    c_identity->lec = (char*) SCR_stracpy((unsigned char*) idt.c_identity->lec);
    c_identity->clec = L_cc(idt.c_identity->lec);
    return *this;
}

bool Identity::operator==(const Identity& other) const
{
    return this->get_lec() == other.get_lec();
}