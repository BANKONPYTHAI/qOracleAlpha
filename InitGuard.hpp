class InitGuard {
    bool initialized = false;
    std::string admin_address;

public:
    InitGuard(const std::string& admin) : admin_address(admin) {}

    bool canOperate(const std::string& sender) const {
        return initialized || sender == admin_address;
    }
    void finishInit(const std::string& sender) {
        if (sender != admin_address) throw "Not admin";
        initialized = true;
    }
};
