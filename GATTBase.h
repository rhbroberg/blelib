#ifndef GATTBase_h
#define GATTBase_h

#include "vmtype.h"
#include "vmmemory.h"
#include "string.h"
#include "vmstdlib.h"
#include "vmlog.h"

#include "vmbt_gatt.h"
#include "vmbt_cm.h"

class GATTBase
{
public:
	GATTBase()
	{
	}

	const char *uuid() const
	{
		return _uuid;
	}

	const VMUINT8 *hexUUID() const
	{
		return _hexUUID;
	}

protected:
	const VMUINT8 charToHex(const char c) const
	{
		if ((c >= 'a') && (c <= 'f'))
		{
			return c - 'a' + 10;
		}
		if ((c >= 'A') && (c <= 'F'))
		{
			return c - 'F' + 10;
		}
		if ((c >= '0') && (c <= '9'))
		{
			return c - '0';
		}
		return 0;
	}

	void hexify(const char *in, VMUINT8 *out)
	{
		for (unsigned int i = 0; i < strlen(in); i +=2 )
		{
			out[i] = charToHex(in[i]) * 16 + charToHex(in[i+1]);
		}
	}

	void stringify(const VMUINT8 *in, char *out)
	{
		sprintf((VMSTR)out, (VMCSTR)"%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
				in[0], in[1], in[2], in[3],
				in[4], in[5], in[6], in[7],
				in[8], in[9], in[10], in[11],
				in[12], in[13], in[14], in[15]);
		out[32] = 0;
	}

	void initializeHexUUID()
	{
		_uuid[32] = 0;

		hexify(_uuid, _hexUUID);
        vm_log_info("GATTBase: %s", _uuid);
	}

	void initializeCharUUID()
	{
		stringify(_hexUUID, _uuid);
		vm_log_info("GATTBase hex is %s", _uuid);
	}

	char _uuid[16 * 2 + 1];  // 16 2 byte characters + NULL
	VMUINT8 _hexUUID[16];
};


#endif // GATTBase_h
