#ifndef CUSTOMVENDOR_H
#define CUSTOMVENDOR_H
extern WorldDatabaseWorkerPool WorldDatabase;

class VendorEntry
{
public:
	int id, vendor, group, next;
	std::string desc;
	VendorEntry(int, int, int, int, std::string);
};

typedef std::list<VendorEntry *> VendorEntryList;

class CustomVendor
{
public:
	VendorEntryList vendorEntryList;
	CustomVendor(void);
	~CustomVendor(void);
	int LoadVendors(void);
	VendorEntryList* GetItemsForEntry(int, int);
	VendorEntryList* GetBaseItemsForEntry(int);
	int GetGroup(int);
	int GetNext(int, int);
	VendorEntry* GetParent(int);
};

extern CustomVendor CustomVendorMgr;

#endif