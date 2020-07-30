#ifndef SdFatJson_h
#define SdFatJson_h

#include "SdFat.h"

class SdFatJson : public FatFile {
   public:
    // bool ls(uint8_t flags, uint8_t indent = 0) {
    //     FatFile file;
    //     if (!isDir() || getError()) {
    //         DBG_FAIL_MACRO;
    //         goto fail;
    //     }
    //     rewind();
    //     while (file.openNext(this, O_RDONLY)) {
    //         if (!file.isHidden() || (flags & LS_A)) {
    //             // indent for dir level
    //             for (uint8_t i = 0; i < indent; i++) {
    //                 //pr->write(' ');
    //             }
    //             if (flags & LS_DATE) {
    //                 //file.printModifyDateTime(pr);
    //                 //pr->write(' ');
    //             }
    //             if (flags & LS_SIZE) {
    //                 //file.printFileSize(pr);
    //                 //pr->write(' ');
    //             }
    //             //file.printName(pr);
    //             //file.getName();
    //             if (file.isDir()) {
    //                 //pr->write('/');
    //             }
    //             //pr->write('\r');
    //             //pr->write('\n');
    //             if ((flags & LS_R) && file.isDir()) {
    //                 if (!file.ls(pr, flags, indent + 2)) {
    //                     DBG_FAIL_MACRO;
    //                     goto fail;
    //                 }
    //             }
    //         }
    //         file.close();
    //     }
    //     if (getError()) {
    //         DBG_FAIL_MACRO;
    //         goto fail;
    //     }
    //     return true;

    // fail:
    //     return false;
    // }

    String getStringName(){
        FatFile file;
        file.openNext(this, O_RDONLY);
        String name = "";
            char buf[255];
            size_t size;
            this->getName(buf, 255);
         //   Serial.write(buf);
        name = String(buf);
        return name;
    }
};  // end of superClass
#endif