/*
 * Z80CPU.h
 *
 *  Created on: 25 июн. 2020 г.
 *      Author: Даниил
 */

#ifndef Z80CPU_H_
#define Z80CPU_H_

#include "Memory.h"

#pragma pack(push, 1)
struct Z80Regs{
	union{
		uint16_t af;
		struct{
			uint8_t f;
			uint8_t a;
		};
	};

	union{
		uint16_t bc;
		struct{
			uint8_t c;
			uint8_t b;
		};
	};

	union{
		uint16_t de;
		struct{
			uint8_t e;
			uint8_t d;
		};
	};

	union{
		uint16_t hl;
		struct{
			uint8_t l;
			uint8_t h;
		};
	};

	union{
		uint16_t ix;
		struct{
			uint8_t ixl;
			uint8_t ixh;
		};
	};

	union{
		uint16_t iy;
		struct{
			uint8_t iyl;
			uint8_t iyh;
		};
	};

	uint16_t pc;
	uint16_t sp;

	uint16_t i;
	uint16_t r;
};
#pragma pack(pop)

class Z80CPU{
protected:
	AddressSpace & _bus;
	Z80Regs _regs {};
	Z80Regs _shadow_regs {};

	bool _iff1 {false};
	bool _iff2 {false};

	unsigned _ticks {0};
	unsigned _state {0};

	uint8_t _prefix {0};
	uint8_t _instruction {0};
	uint8_t _wait {0};

	uint8_t get_reg(uint8_t) const;
public:
	Z80CPU(AddressSpace & bus): _bus(bus){}

	void tick();
	void reset();

	void save_state_sna(const char * filename);
	void load_state_sna(const char * filename);
	void load_state_z80(const char * filename);

};




#endif /* Z80CPU_H_ */
