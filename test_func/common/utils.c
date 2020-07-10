int compare_poly_ram(poly *r, uint64_t *hls_r, const char *string)
{
    printf("%s:\n", string);
    uint16_t a[4];
    uint16_t b[4];
    bool error = false;
    for (uint16_t i = 0; i < NEWHOPE_N; i += 4)
    {
        a[0] = r->coeffs[i];
        a[1] = r->coeffs[i + 1];
        a[2] = r->coeffs[i + 2];
        a[3] = r->coeffs[i + 3];

        unpack(hls_r, i / 4, &b[0], &b[1], &b[2], &b[3]);

        for (uint16_t j = 0; j < 4; j++)
        {
            if (a[j] != b[j])
            {
                printf("[%u] %u != %u\n", i + j, a[j], b[j]);
                error = true;
            }
        }
        if (error)
        {
            printf("Result: Failed!\n");
            printf("+++++++++++++++\n");
            exit(1);
            return 1;
        }
    }
    printf("Result: Success!\n---------------------\n");
    return 0;
}
