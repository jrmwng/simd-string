int sse42_memcmp(void* s1, void* s2, size_t n) {

    if (n == 0 || (s1 == s2)) {
        return 0;
    }

    __m128i* ptr1 = reinterpret_cast<__m128i*>(s1);
    __m128i* ptr2 = reinterpret_cast<__m128i*>(s2);
    
    __m128i ub16NextA = _mm_loadu_si128(ptr1);
    __m128i ub16NextB = _mm_loadu_si128(ptr2);

    const __m128i b16Const128 = _mm_set1_epi8(-128);
    
    const uint8_t mode =
        _SIDD_UBYTE_OPS |
        _SIDD_CMP_EQUAL_EACH |
        _SIDD_NEGATIVE_POLARITY |
        _SIDD_LEAST_SIGNIFICANT;

    for (; n > 0; ptr1++, ptr2++, n-=16)
    {
        const __m128i ub16A = ub16NextA; ub16NextA = _mm_loadu_si128(ptr1);
        const __m128i ub16B = ub16NextB; ub16NextB = _mm_loadu_si128(ptr2);

        if (_mm_cmpestrc(a, n, b, n, mode))
        {
            const int nIndex = _mm_cmpestri(a, n, b, n, mode);
                
            const __m128i b16A = _mm_add_epi8(ub16A, b16Const128);
            const __m128i b16B = _mm_add_epi8(ub16B, b16Const128);
                
            const __m128i b16AcmpltB = _mm_cmplt_epi8(b16A, b16B);
            const __m128i b16BcmpltA = _mm_cmplt_epi8(b16B, b16A);
        
            const int nAcmpltB = (_mm_movemask_epi8(b16AcmpltB) >> nIndex) & 1;
            const int nBcmpltA = (_mm_movemask_epi8(b16BcmpltA) >> nIndex) & 1;

            return nBcmpltA - nAcmpltB;
        }
    }

    return 0;
}
