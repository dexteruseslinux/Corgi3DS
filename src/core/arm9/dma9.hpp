#ifndef DMA9_HPP
#define DMA9_HPP
#include <cstdint>

struct XDMA_Chan_CTRL
{
    uint8_t endian_swap_size;
    bool inc_dest;
    bool inc_src;
};

struct XDMA_Chan
{
    enum Status
    {
        STOP,
        EXEC,
        CACHEMISS,
        UPDATEPC,
        WFE,
        BARRIER,
        WFP = 7,
        KILL,
        COMPLETE,
        FAULTCOMPLETE = 0xE,
        FAULT
    };

    XDMA_Chan_CTRL ctrl;

    Status state;
    uint32_t PC;
    uint32_t source_addr, dest_addr;
    uint16_t loop_ctr[2];
};

enum NDMA_Request
{
    NDMA_TIMER0,
    NDMA_TIMER1,
    NDMA_TIMER2,
    NDMA_TIMER3,
    NDMA_CTRCARD0,
    NDMA_CTRCARD1,
    NDMA_EMMC = 7,
    NDMA_AES_WRITEFREE,
    NDMA_AES_READFREE,
    NDMA_SHA,
    NDMA_AES2 = 15
};

struct NDMA_Chan
{
    uint32_t source_addr, dest_addr;

    //Internal registers that source and dest are copied to; incremented during operation
    uint32_t int_src, int_dest;
    uint32_t transfer_count;
    uint32_t write_count;
    uint32_t fill_data;

    uint8_t dest_update_method, src_update_method;
    bool dest_reload, src_reload;
    uint8_t words_per_block;

    NDMA_Request startup_mode;
    bool imm_mode;
    bool repeating_mode;
    bool irq_enable;
    bool busy;
};

class Emulator;

class DMA9
{
    private:
        Emulator* e;

        NDMA_Chan ndma_chan[8];

        //XDMA channel 8 is the DMAC manager (unknown how this is different from other channels)
        XDMA_Chan xdma_chan[8];
        uint32_t xdma_ie, xdma_if;

        uint32_t xdma_debug_instrs[2];
        bool xdma_command_set;
        uint8_t xdma_command;
        uint8_t xdma_params[0x10];
        uint8_t xdma_param_count;
        uint8_t xdma_params_needed;

        void run_ndma(int chan);

        void instr_lp(int chan, int loop_ctr_index);
        void instr_wfp(int chan);
        void instr_flushp();
        void instr_dmago();
        void instr_mov(int chan);

        void xdma_write_chan_ctrl(int chan, uint32_t value);

        void xdma_exec_debug();
        void xdma_exec_instr(uint8_t byte, int chan);
    public:
        DMA9(Emulator* e);

        void reset();
        void run_xdma();
        void ndma_req(NDMA_Request req);

        uint32_t read32_ndma(uint32_t addr);
        uint32_t read32_xdma(uint32_t addr);
        void write32_ndma(uint32_t addr, uint32_t value);
        void write32_xdma(uint32_t addr, uint32_t value);
};

#endif // DMA9_HPP
