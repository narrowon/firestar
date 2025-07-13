#include "../../../hooks.hpp"

MAKE_HOOK(
	CSequenceTransitioner_CheckForSequenceChange,
	s::CSequenceTransitioner_CheckForSequenceChange.get(),
	void,
	void* rcx, CStudioHdr* hdr, int nCurSequence, bool bForceNewSequence, bool bInterpolate)
{
	CALL_ORIGINAL(rcx, hdr, nCurSequence, bForceNewSequence, false);
}