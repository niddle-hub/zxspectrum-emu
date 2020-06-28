#include "Z80CPU.h"
#include <cstdint>
#include <vector>
#include <fstream>

#pragma pack(push, 1)
struct SNA_Header
{
	uint8_t I;
	uint16_t HL1;
	uint16_t DE1;
	uint16_t BC1;
	uint16_t AF1;
	uint16_t HL;
	uint16_t DE;
	uint16_t BC;
	uint16_t IY;
	uint16_t IX;
	uint8_t IFF2;
	uint8_t R;
	uint16_t AF;
	uint16_t SP;
	uint8_t IM;
	uint8_t FE;

};
#pragma pack(pop)

#pragma pack(push, 1)
struct Z80_Header_1
{
	uint8_t A, F;
	uint8_t C, B;
	uint8_t L, H;
	uint16_t PC;
	uint16_t SP;
	uint8_t I, R;
	uint8_t stuffs1;
	uint8_t E, D;
	uint8_t C1, B1;
	uint8_t E1, D1;
	uint8_t L1, H1;
	uint8_t A1, F1;
	uint8_t IYL, IXH;
	uint8_t IFF1, IFF2;
	uint8_t stuffs2;

};
#pragma pack(pop)

#pragma pack(push, 1)
struct Z80_Header_2
{
	//uint16_t hdrlen;
	uint16_t PC;
	uint8_t hw;
	uint8_t ext0; //7ffd
	uint8_t ext1; //If.1
	uint8_t ext2; //ayyy
	uint8_t ay_last_write;
	uint8_t ay_state[16];
	uint16_t tsc_lo;
	uint8_t tsc_hi;
	uint8_t zero;
	uint8_t whatever[4];
	uint8_t joy[10];
	uint8_t joy2[10];
	uint8_t fsck_those_bytes[4];

};
#pragma pack(pop)

void Z80CPU::save_state_sna(const char* filename)
{
	SNA_Header hdr;
	std::vector<uint8_t> data;
	data.resize(16384*3);
	for(unsigned memptr=16384; memptr<65536; memptr++)
		data[memptr-16384]=_bus.read(memptr);

		hdr.I= _z80._context.I;
		hdr.HL1=_z80._context.R2.wr.HL;
		hdr.DE1=_z80._context.R2.wr.DE;
		hdr.BC1=_z80._context.R2.wr.BC;
		hdr.AF1=_z80._context.R2.wr.AF;
		hdr.HL=_z80._context.R1.wr.HL;
		hdr.DE=_z80._context.R1.wr.DE;
		hdr.BC=_z80._context.R1.wr.BC;
		hdr.IY=_z80._context.R1.wr.IY;
		hdr.IX=_z80._context.R1.wr.IX;
		hdr.IFF2=_z80._context.IFF2;
		hdr.R=_z80._context.R;
		hdr.AF=_z80._context.R1.wr.AF;
		hdr.SP=_z80._context.R1.wr.SP;
		hdr.IM=_z80._context.IM;
		hdr.FE=0;
		hdr.SP-=2;
		data[hdr.SP-16384]=_z80.context.PC & 0x00ff;
		data[hdr.SP-16384+1]=_z80.context.PC & >>8;

		std::fstream sna;
		sna.open(filename, std::ios::out | std::ios::binary);
		sna.write(reinterpret_cast<const char *>(&hdr), sizeof(hdr));
		sna.write(reinterpret_cast<const char *>(&data[0]), data.size());
		sna.close();
}

void Z80CPU::load_state_sna(const char* filename)
{
	SNA_Header hdr;
		std::vector<uint8_t> data;
		data.resize(16384*3);

		std::fstream sna;
		sna.open(filename, std::ios::in | std::ios::binary);
		sna.read(reinterpret_cast<char *>(&hdr), sizeof(hdr));
		sna.read(reinterpret_cast<char *>(&data[0]), data.size());

		data[hdr.SP-16384]=_z80.context.PC & 0x00ff;
		data[hdr.SP-16384+1]=_z80.context.PC & >>8;

		_z80._context.PC=0;
		_z80._context.PC | =data[hdr.SP - 16384];
		_z80._context.PC | =(data[hdr.SP - 16384+1]<<8);
		hdr.SP+=2;

		for(unsigned memptr=16384; memptr<65536; memptr++)
			_bus.write(memptr, data[memptr-16384]);

				_z80._context.I=hdr.I;
				_z80._context.R2.wr.HL=hdr.HL1;
				_z80._context.R2.wr.DE=hdr.DE1;
				_z80._context.R2.wr.BC=hdr.BC1;
				_z80._context.R2.wr.AF=hdr.AF1;
				_z80._context.R1.wr.HL=hdr.HL;
				_z80._context.R1.wr.DE=hdr.DE;
				_z80._context.R1.wr.BC=hdr.BC;
				_z80._context.R1.wr.IY=hdr.IY;
				_z80._context.R1.wr.IX=hdr.IX;
				_z80._context.IFF2=hdr.IFF2;
				_z80._context.R=hdr.R;
				_z80._context.R1.wr.AF=hdr.AF;
				_z80._context.R1.wr.SP=hdr.SP;
				_z80._context.IM=hdr.IM;

				_bus.write(0xfe, hdr.FE,true);
				_z80._context.IFF1=_80._context.IFF2;
}

void Z80CPU::load_state_z80(const char* filename) {
	[[maybe_unused]] int version = 0;
	uint16_t real_pc;
	Z80_Header_1 hdr1;
	Z80_Header_2 hdr2;

	std::vector<uint8_t> data;
	data.resize(16384*3);

	std::fstream z80f;
	z80f.open(filename, std::ios::in | std::ios::binary);
	z80f.read(reinterpret_cast<char *>(&hdr1), sizeof(hdr1));
	if(hdr1.PC==0)
	{
		version =2;
		uint16_t hdr2size;
		z80f.read(reinterpret_cast<char *>(&hdr2size), 2);
		z80f.read(reinterpret_cast<char *>(&hdr2), hdr2size);

	} else {
		real_pc=hdr1.PC;
	}

		_z80._context.R1.br.A= hdr1.A;
		_z80._context.R1.br.F= hdr1.F;
		_z80._context.R1.br.C= hdr1.C;
		_z80._context.R1.br.B= hdr1.B;
		_z80._context.R1.br.L= hdr1.L;
		_z80._context.R1.br.H= hdr1.H;
		_z80._context.PC=real_pc;
		_z80._context.R1.wr.SP=hdr1.SP;
		_z80._context.I =hdr1.I;
		_z80._context.R =hdr1.R;
		_z80._context.R1.br.E= hdr1.E;
		_z80._context.R1.br.D= hdr1.D;
		_z80._context.R1.br.C= hdr1.C1;
		_z80._context.R1.br.B= hdr1.B1;
		_z80._context.R1.br.E= hdr1.E1;
		_z80._context.R1.br.D= hdr1.D1;
		_z80._context.R1.br.L= hdr1.L1;
		_z80._context.R1.br.H= hdr1.H1;
		_z80._context.R1.br.A= hdr1.A1;
		_z80._context.R1.br.F= hdr1.F1;
		_z80._context.R1.br.IYl= hdr1.IYL;
		_z80._context.R1.br.IYh= hdr1.IYH;
		_z80._context.R1.br.IXl= hdr1.IXL;
		_z80._context.R1.br.IXh= hdr1.IXH;
		_z80._context.IFF1= hdr1.IFF1;
		_z80._context.IFF2= hdr1.IFF2;

		_bus.write(0xfe, (hdr1.stuffs1>>1)& 0x07, true);
		_z80._context.IM=hdr1.stuffs2 & 0x03;

		if(hdr1.stuffs1 & 0x20){ //������ �����
			uint16_t memptr =0;
			uint8_t b1, b2, xx, yy;

			do{
				z80f.read(reinterpret_cast<char *>(&b1), 1);
			if(b1!=0xed){
				data[memptr]= b1;
				memptr++;
			}else {
				z80f.read(reinterpret_cast<char *>(&b2), 1);
				if(b2 != 0xed){
					data[memptr]= b1;
					memptr++;
					data[memptr]= b2;
					memptr++;
				}else{
					z80f.read(reinterpret_cast<char *>(&xx), 1);
					z80f.read(reinterpret_cast<char *>(&yy), 1);
					while(yy>0){
						data[memptr++]=xx;
					}
				}
			}
			}while(z80f.good() and not z80f.eof());
		} else {
			//������ �� �����
			z80f.readsome(reinterpret_cast<char *>(&data[0]), data.size());
		}

		for(uint16_t memptr = 0; memptr<data.size(); memptr++)
			_bus.write(memptr+16384, data[memptr]);

	z80f.close();
}
