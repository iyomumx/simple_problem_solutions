#include <stdio.h>
#include <stdlib.h>

#define MAX_N 1000000

typedef long long LL;

LL table[MAX_N] = {0, 1};

LL sod(int n)
{
	if (table[n] != 0) return table[n];
	int p, q, c;
	for (p = 2; p < n / 2; p++)
	{
		if ( n % p == 0 ) break;
	}
	if (n % p != 0)
	{
		table[n] = 1 + n;
		return table[n];
	}
	else
	{
		int pn = p;
		LL sum = 1 + p;
		while ((q = n / pn) % p == 0)
		{
			pn *= p;
			sum += pn;
		}
		table[pn] = sum;
		table[q] = sod(q);
		table[n] = table[pn] * table[q];
		return table[n];
	}
}

LL badness_of(int n)
{
	LL ret = sod(n) - 2 * n;
	if (ret < 0) ret = -ret;
	return ret;
}

int main(void)
{
	int start, end, badness, n = 1;
	while(scanf("%d%d%d", &start, &end, &badness) == 3)
	{
		if ((start == 0) && (end == 0) && (badness == 0))
		{
			break;
		}
		int count = 0, i;
		for (i = start; i <= end; i++)
		{
			count += badness_of(i) <= badness;
		}
		printf("Test %d: %d\n", n++, count);
	}
	return 0;
}