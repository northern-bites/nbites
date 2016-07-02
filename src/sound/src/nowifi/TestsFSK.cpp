#include "SimpleFSK.hpp"
#include "SimpleFSK.hpp"
#include "utilities-test.hpp"
#include "../sound/Transform.hpp"
#include "Correlation.hpp"

using namespace nbsound;
using namespace nowifi;

NBL_ADD_TEST_TO(simple_fsk_send, fsk) {
    Config config{48000, 8192};
    SampleBuffer full_buffer{1, SIMPLE_FSK_WINDOW_SIZE * 9};

    uint8_t alt = 0x55;
    std::string data((char *) &alt, 1);

    SimpleFSKSendr test;
    test.send(data);

    int cycle = 0;
    while (test.isSending()) {
        SampleBuffer window = full_buffer.window(cycle * SIMPLE_FSK_WINDOW_SIZE, (cycle + 1) * SIMPLE_FSK_WINDOW_SIZE);

        test.fill(window, config);
    }

    return true;
}

NBL_ADD_TEST_TO(corr_and_transform, fsk) {
    Config config{48000, 8192};
    SampleBuffer buffer{1, 8192};

    nowifi::CorrSender corr(nowifi::SIMPLE_FSK_START_F);

    corr.fill(buffer, config);

    Transform transform(8192);
    transform.transform(buffer, 0);

    float expected = transform.bin_for_frequency(SIMPLE_FSK_START_F, config.sample_rate);

    printf("freq len %d\n", transform.get_freq_len());
    printf("bin expected at %f\n",
           expected);

    int max = 0;
    float fmax = 0;
    for (int i = 0; i < transform.get_freq_len(); ++i) {
        if (transform.get(i) > fmax) {
            max = i;
            fmax = transform.get(i);
        }
    }

    printf("max %f @ %d, expected %f\n",
           fmax, max, expected);

    NBL_ASSERT(fabs(max - expected) < 1.0);


    return true;
}

NBL_ADD_TEST_TO(transform, fsk) {
    Config config{48000, 4096};
    SampleBuffer buffer{1, 4096};
    nowifi::CorrSender sender;

    Transform * transform = new Transform(4096);
    sender.fill(buffer, config);

    transform->transform(buffer, 0);

    //    for (int i = 0; i < transform->get_freq_len(); ++i) {
    //        printf("%d %f\n", i, transform->get(i));
    //    }

    printf("bins %lf %lf\n",
           transform->bin_for_frequency(8192, 48000),
           transform->bin_for_frequency(19000, 48000) );

    printf("freqs %lf %lf\n",
           transform->frequency_at(350, 48000),
           transform->frequency_at(811, 48000) );

    delete transform;

    return true;
}

bool offset_test(int test_off, Config& config, SampleBuffer& full_buffer) {

    NBL_WARN("testing offset %d", test_off)

    memset(full_buffer.buffer, 0, full_buffer.size_bytes());
    NBL_INFO("zero fill done");

    uint8_t alt = 0x55;
    std::string data((char *) &alt, 1);

    SimpleFSKSendr test;
    test.send(data);

    SampleBuffer window = full_buffer.window(SIMPLE_FSK_WINDOW_SIZE * 4 + test_off, SIMPLE_FSK_WINDOW_SIZE * 5 + test_off);
    SampleBuffer window2 = full_buffer.window(SIMPLE_FSK_WINDOW_SIZE * 5 + test_off, SIMPLE_FSK_WINDOW_SIZE * 6 + test_off);

    test.fill(window, config);
    test.fill(window2, config);

    int end = 0, start = 0;
    for (int i = 0; i < full_buffer.size_samples(); ++i) {
        if (full_buffer[0][i]) {
            printf("start at %d %d\n", i, full_buffer[0][i]);
            start = i;
            break;
        }
    }

    for (int i = full_buffer.size_samples(); i > 0; --i) {
        if (full_buffer[0][i]) {
            printf("end at %d %d\n", i, full_buffer[0][i]);
            end = i;
            break;
        }
    }

    printf("length = %d\n", end - start);
    NBL_ASSERT(abs(end - start - (2*SIMPLE_FSK_WINDOW_SIZE)) < 5)

    NBL_INFO("offset fill done");
    SimpleFSKRecvr recvr(nullptr);
    recvr.NUM_FRAMES_TO_CALIBRATE = 2;
    //    NBL_INFO("after constructor");

    for (int i = 0; i < 9; ++i) {
        //        NBL_INFO("loop %d", i);
        SampleBuffer window = full_buffer.window(i * SIMPLE_FSK_WINDOW_SIZE, (i + 1) * SIMPLE_FSK_WINDOW_SIZE);

        recvr.parse(window, config);
        if (recvr.current == RECORDING) break;
    }

    int offset = (int) recvr.offset();
    int diff = (offset - test_off);

    NBL_ASSERT(offset < SIMPLE_FSK_WINDOW_SIZE)
    return abs(diff) < 5;
}

NBL_ADD_TEST_TO(sfsk_recv1, fsk) {

    Config config{48000, 8192};
    SampleBuffer full_buffer{1, SIMPLE_FSK_WINDOW_SIZE * 9};

    for (float i = 0; i < 10; ++i) {
        NBL_ASSERT( offset_test( (i / 10) * SIMPLE_FSK_WINDOW_SIZE, config, full_buffer) );
    }

    return true;
}


NBL_ADD_TEST_TO(sfsk_parse1, fsk) {

    Config config{48000, 8192};
    SampleBuffer full_buffer{1, SIMPLE_FSK_WINDOW_SIZE * 10};
    SampleBuffer empty{1, SIMPLE_FSK_WINDOW_SIZE};
    memset(empty.buffer, 0, empty.size_bytes());

    uint8_t alt = 0x55;
    std::string data((char *) &alt, 1);

    SimpleFSKSendr test;
    test.send(data);

    nbl::utilities::print_divider(1, 1, NULL);

    for (int cycle = 0; test.isSending(); ++cycle) {
        NBL_WARN("send loop %d", cycle)
        SampleBuffer window = full_buffer.window(cycle * SIMPLE_FSK_WINDOW_SIZE, (cycle + 1) * SIMPLE_FSK_WINDOW_SIZE);

        test.fill(window, config);
    }

    nbl::utilities::print_divider(1, 1, NULL);

    SimpleFSKRecvr recvr{nullptr};
    recvr.NUM_FRAMES_TO_CALIBRATE = 2;

    for (int i = 0; i < 2; ++i) {
        recvr.parse(empty, config);
    }

    nbl::utilities::print_divider(1, 1, NULL);

    for (int i = 0; i < 10; ++i) {
        SampleBuffer window = full_buffer.window(i * SIMPLE_FSK_WINDOW_SIZE, (i + 1) * SIMPLE_FSK_WINDOW_SIZE);
        
        recvr.parse(window, config);
    }

    std::string rdata = recvr.data_so_far();
    NBL_ASSERT(rdata.size() == 1);
    printf("0x%x\n", rdata[0] );

    NBL_ASSERT(rdata[0] == alt);
    
    return true;
}

std::string TEST_RETURN;

void the_callback(time_t start, time_t end, std::string data) {
    TEST_RETURN = data;
}

NBL_ADD_TEST_TO(sfsk_parse2, fsk) {

    srand(1990);

    Config config{48000, 8192};
    SampleBuffer full_buffer{1, SIMPLE_FSK_WINDOW_SIZE * 50};

    int i;

    for (i = 0; i < 3; ++i) {
        SampleBuffer window = full_buffer.window(i * SIMPLE_FSK_WINDOW_SIZE, (i + 1) * SIMPLE_FSK_WINDOW_SIZE);

        for (int i = 0; i < window.size_samples(); ++i) {
            window[0][i] = rand() % window.sample_max();
        }
    }

    std::string data("t");

    SimpleFSKSendr test;
    test.send(data);

    nbl::utilities::print_divider(1, 1, NULL);

    for (; test.isSending(); ++i) {
        NBL_ASSERT_LT(i, 50)
        NBL_WARN("send loop %d", i)
        SampleBuffer window = full_buffer.window(i * SIMPLE_FSK_WINDOW_SIZE, (i + 1) * SIMPLE_FSK_WINDOW_SIZE);

        test.fill(window, config);
    }

    nbl::utilities::print_divider(1, 1, NULL);

    SimpleFSKRecvr recvr{the_callback};
    recvr.NUM_FRAMES_TO_CALIBRATE = 2;

    nbl::utilities::print_divider(1, 1, NULL);

    for (int i = 0; i < 50; ++i) {
        SampleBuffer window = full_buffer.window(i * SIMPLE_FSK_WINDOW_SIZE, (i + 1) * SIMPLE_FSK_WINDOW_SIZE);

        recvr.parse(window, config);
    }

    NBL_WARN("got result: %s", TEST_RETURN.c_str())

    NBL_ASSERT_EQ(TEST_RETURN, data)

    return true;
}
