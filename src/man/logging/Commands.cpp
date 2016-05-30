//
//  Commands.cpp
//  tool8-separate
//
//  Created by Philip Koch on 4/14/16.
//

#include "Control.hpp"
#include "LogRPC.hpp"

using nbl::SharedConstants;

namespace control {
    //helper functions may be defined here

    RPC_MAKE_FUNCTION_GROUP(Control)

    nbl::logptr createFlagStateLog() {
        json::Array flags;
        for (int i = flags::_START_EXTERNAL_ + 1; i < flags::_END_EXTERNAL_; ++i) {
            flags::flag_e f = (flags::flag_e) i;
            json::Object attrs;
            attrs["index"] = json::Number(f);
            attrs["value"] = json::Boolean(check(f));
            attrs["name"] = json::String(stringFor(f));

            flags.push_back(attrs);
        }

        nbl::logptr flagLog = nbl::Log::explicitLog({}, {}, nbl::SharedConstants::LogClass_Flags());

        flagLog->addBlockFromJson(flags, "createFlagStateLog()", nbl::IMAGE_INDEX_INVALID, clock());

        return flagLog;
    }

#define CONTROL_FUNCTION(name, va, ...) \
    RPC_FUNCTION_DECLARE_COMPLETE(name, va, ## __VA_ARGS__) \
    RPC_FUNCTION_STATIC_ADD(name, RPC_FUNCTION_INSTANTIATE(name), Control)\
    RPC_FUNCTION_DEFINE(name)

    CONTROL_FUNCTION(SetFlag, false, SharedConstants::LogClass_Null())
        (const std::vector<nbl::logptr> &arguments) {
            NBL_PRINT("RPCFunctionSetFlag::call()");
            uint8_t index = arguments[0]->blocks[0].data[0];
            uint8_t value = arguments[0]->blocks[0].data[1];
            NBL_PRINT("RPCFunctionSetFlag: %s -> %d",
                      stringFor((flags::flag_e) index), value);

            set( (flags::flag_e) index, value);
            RETURN(createFlagStateLog());
        }

    CONTROL_FUNCTION(GetFlags, false)
        (const std::vector<nbl::logptr> &arguments) {
            NBL_PRINT("RPCFunctionGetFlag::call()");
            RETURN(createFlagStateLog());
        }

}

#if !defined(NBL_STANDALONE) && !defined(__APPLE__)
#include "DebugConfig.h"
#include <CameraParams.pb.h>

#include <sstream>
#include <iostream>
#include <fstream>
#include <exception>

using nbl::SExpr;

namespace control {

    void write_params(messages::CameraParams& params) {
        SExpr s;

        SExpr h = SExpr("hflip",params.h_flip());
        SExpr v = SExpr("vflip",params.v_flip());
        SExpr ae = SExpr("autoexposure",params.auto_exposure());
        SExpr b = SExpr("brightness",params.brightness());
        SExpr c = SExpr("contrast",params.contrast());
        SExpr sat = SExpr("saturation",params.saturation());
        SExpr hue = SExpr("hue",params.hue());
        SExpr sharp = SExpr("sharpness",params.sharpness());
        SExpr gamma = SExpr("gamma",params.gamma());
        SExpr awb = SExpr("auto_whitebalance",params.autowhitebalance());
        SExpr expo = SExpr("exposure",params.exposure());
        SExpr gain = SExpr("gain",params.gain());
        SExpr wb = SExpr("white_balance",params.whitebalance());
        SExpr ftb = SExpr("fade_to_black",params.fadetoblack());

        s.append(h);
        s.append(v);
        s.append(ae);
        s.append(b);
        s.append(c);
        s.append(sat);
        s.append(hue);
        s.append(sharp);
        s.append(gamma);
        s.append(awb);
        s.append(expo);
        s.append(gain);
        s.append(wb);
        s.append(ftb);

        std::string stringToWrite = s.serialize();

#ifdef V5_ROBOT
        std::cout<<"Saving as V5"<<std::endl;
        if(params.whichcamera() == "TOP"){
            std::cout<<"TOP Params Received"<<std::endl;
            std::ofstream file("/home/nao/nbites/Config/V5topCameraParams.txt");
            std::cout<<stringToWrite<<std::endl;
            file << stringToWrite;
            file.close();
            std::cout<<"Saving Done"<<std::endl;
        } else  {
            std::cout<<"Bottom Params Received"<<std::endl;
            std::ofstream file("/home/nao/nbites/Config/V5bottomCameraParams.txt");
            std::cout<<stringToWrite<<std::endl;
            file << stringToWrite;
            file.close();
            std::cout<<"Saving Done"<<std::endl;
        }
#else
        std::cout<<"Saving as V4"<<std::endl;
        if(params.whichcamera() == "TOP"){
            std::cout<<"TOP Params Received"<<std::endl;
            std::ofstream file("/home/nao/nbites/Config/V4topCameraParams.txt");
            file << stringToWrite;
            file.close();
            std::cout<<"Saving Done"<<std::endl;
        } else  {
            std::cout<<"Bottom Params Received"<<std::endl;
            std::ofstream file("/home/nao/nbites/Config/V4bottomCameraParams.txt");
            file << stringToWrite;
            file.close();
            std::cout<<"Saving Done"<<std::endl;
        }
#endif
    }

    CONTROL_FUNCTION(SetCameraParams, false, "CameraParamsPair")
        (const std::vector<nbl::logptr> &arguments) {
            NBL_PRINT("SetCameraParams()");

            messages::CameraParams topParams;
            messages::CameraParams botParams;

            bool success;
            success = arguments[0]->blocks[0].parseAsProtobuf(topParams);
            NBL_ASSERT(success)
            success = arguments[0]->blocks[1].parseAsProtobuf(botParams);
            NBL_ASSERT(success)

            write_params(topParams);
            write_params(botParams);
            NBL_PRINT("SetCameraParams() done.");
        }
}
#endif




