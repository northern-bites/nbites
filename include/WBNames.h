#ifndef WBNames_h
#define WB_Names_h


namespace WBNames{

    static const unsigned int NUM_FSR = 8;

    enum FSR_INDICES {
        LFSR_FL = 0,
        LFSR_FR,
        LFSR_RL,
        LFSR_RR,
        RFSR_FL,
        RFSR_FR,
        RFSR_RL,
        RFSR_RR,
    };


    static const string FSR_CORE[NUM_FSR] = {
        "LFsrFL","LFsrFR","LFsrBL","LFsrBR",
        "RFsrFL","RFsrFR","RFsrBL","RFsrBR"
    };

}
#endif
