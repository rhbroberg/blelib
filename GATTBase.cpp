#include "GATTBase.h"

GATTBase::GATTBase()
{
}

const char *
GATTBase::uuid() const
{
	return _uuid;
}

const VMUINT8 *
GATTBase::hexUUID() const
{
	return _hexUUID;
}

const VMUINT8
GATTBase::charToHex(const char c) const
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

void
GATTBase::hexify(const char *in, VMUINT8 *out) const
{
	for (unsigned int i = 0; i < strlen(in); i +=2 )
	{
		out[i] = charToHex(in[i]) * 16 + charToHex(in[i+1]);
	}
}

void
GATTBase::stringify(const VMUINT8 *in, char *out) const
{
	sprintf((VMSTR)out, (VMCSTR)"%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
			in[0], in[1], in[2], in[3],
			in[4], in[5], in[6], in[7],
			in[8], in[9], in[10], in[11],
			in[12], in[13], in[14], in[15]);
	out[32] = 0;
}

void
GATTBase::initializeHexUUID()
{
	_uuid[32] = 0;

	hexify(_uuid, _hexUUID);
	vm_log_info("GATTBase: %s", _uuid);
}

void
GATTBase::initializeCharUUID()
{
	stringify(_hexUUID, _uuid);
	vm_log_info("GATTBase hex is %s", _uuid);
}
