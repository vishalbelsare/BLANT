#include "combin.h"
#include "tinygraph.h"
#include "blant.h"
#include "misc.h"
#include <stdio.h>

int _alphaList[MAX_CANONICALS];
Gint_type _canonList[MAX_CANONICALS];
int _canonNumEdges[MAX_CANONICALS];

int CountPath(TINY_GRAPH* g, TSET seen, TSET candidates, int k) {
    seen = TSetUnion(seen, candidates);

    int outbound[((int)(k+1)/2)*((int)k/2)], outboundSize = 0;
    if (TSetCardinality(seen) == k) return 1;

    //Fill outbound for seen set
    int i, j;
    for (i = 0; i < k; i++) {
        if (!TSetIn(seen, i)) continue;

        for (j = 0; j < k; j++) {
            if (i != j && !TSetIn(seen, j) && TinyGraphAreConnected(g, i, j)) {
                outbound[outboundSize++] = j;
            }
        }
    }

    int calculatedValue[k];
    for (i = 0; i < k; i++) calculatedValue[i] = 0;

    //Recurse through neighbors in outbound
    int total = 0;
    for (i = 0; i < outboundSize; i++) {
        if (!calculatedValue[outbound[i]]) {
            TSetEmpty(candidates);
            TSetAdd(candidates, outbound[i]);
            calculatedValue[outbound[i]] = CountPath(g, seen, candidates, k);
        }
        total += calculatedValue[outbound[i]];
    }
    return total;
}

int ComputeAlphaNode(TINY_GRAPH* g, int k) {
    int total = 0, i, j;
    TSET seen = 0, candidates;
    for (i = 0; i < k; i++) {
        for (j = i+1; j < k; j++) {
            if (!TinyGraphAreConnected(g, i, j))
                continue;

            TSetEmpty(candidates);
            TSetAdd(candidates, i);
            TSetAdd(candidates, j);

            total += CountPath(g, seen, candidates, k);
            TSetEmpty(candidates);
        }
    }
    return total;
}

SET* _connectedCanonicals;
int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "USAGE: %s k\n", argv[0]);
        exit(-1);
    }
    int k = atoi(argv[1]);
    char BUF[BUFSIZ];
#if SELF_LOOPS
    TINY_GRAPH *g = TinyGraphSelfAlloc(k);
#else
    TINY_GRAPH *g = TinyGraphAlloc(k);
#endif
    _connectedCanonicals = canonListPopulate(BUF, _canonList, k, _canonNumEdges);
    int numCanon = _connectedCanonicals->maxElem;
    int i;
    for (i = 0; i < numCanon; i++) {
	    Int2TinyGraph(g, _canonList[i]);
    if (!SetIn(_connectedCanonicals, i))
	_alphaList[i] = 0;
	    else
	_alphaList[i] = ComputeAlphaNode(g, k);
    }

    printf("%d\n", numCanon);
    for (i = 0; i < numCanon; i++) {
	    printf("%d ", _alphaList[i]);
    }
    printf("\n");

    TinyGraphFree(g);
    SetFree(_connectedCanonicals);

    return 0;
}