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
	GATTBase();

	const char *uuid() const;
	const VMUINT8 *hexUUID() const;

protected:
	const VMUINT8 charToHex(const char c) const;
	void hexify(const char *in, VMUINT8 *out);
	void stringify(const VMUINT8 *in, char *out);
	void initializeHexUUID();
	void initializeCharUUID();

	char _uuid[16 * 2 + 1];  // 16 2 byte characters + NULL
	VMUINT8 _hexUUID[16];
};

#endif // GATTBase_h
