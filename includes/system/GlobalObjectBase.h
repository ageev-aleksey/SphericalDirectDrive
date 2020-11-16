//
// Created by Aleksey on 17.03.2020.
//

#ifndef SPPU_SERIAL_GLOBALOBJECTBASE_H
#define SPPU_SERIAL_GLOBALOBJECTBASE_H

class GlobalObjectBase {
public:
    GlobalObjectBase();
    virtual ~GlobalObjectBase();
    size_t getHashCode();
    void  reCalcHashCode();
protected:
    size_t hash_code_object;
};


#endif //SPPU_SERIAL_GLOBALOBJECTBASE_H
