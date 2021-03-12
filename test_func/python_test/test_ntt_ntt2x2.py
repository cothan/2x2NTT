#!/usr/bin/env python3

import copy
import random

from test_input import INPUT as data_a, NEWHOPE_N, NEWHOPE_Q, logN, omega_table, psis_table

DEBUG = 0
DEBUG1 = 0
DEBUG2 = 0
DEBUG3 = 0

less_32 = 16
less_64 = 100111


def sort_and_combine(topram):
    topram = list(map(int, topram))
    ram1 = []
    ram2 = []
    ram3 = []
    ram4 = []
    for i in position_1:
        ram1.append(topram[i])
    for i in position_2:
        ram2.append(topram[i])
    for i in position_3:
        ram3.append(topram[i])
    for i in position_4:
        ram4.append(topram[i])
    return [ram1, ram2, ram3, ram4]


def print_mulcol(topram):
    line = ['', '', '', ''] * 4
    for i in range(len(topram)):
        temp = list(map(str, topram[i]))
        for j in range(nocoln):
            line[i] += '{0: <8}'.format(temp[j])
    for i in line:
        print(i)


def print_sel_mulcol(topram, list):
    line = ['', '', '', ''] * 4
    for i in range(len(topram)):
        temp = list(map(str, topram[i]))
        for j in list:
            line[i] += '{0: <8}'.format(temp[j])
    for i in line:
        print(i)


def pad(a):
    temp = bin(a).lstrip('0b')
    temp = '0' * (logN - len(temp)) + temp
    return temp


def rev(a):
    """
    return bit reverse of a
    """
    a = pad(a)
    a = a[::-1]
    return int(a, 2)


def bit_rev(array):
    out = [0 for i in range(NEWHOPE_N)]
    for i in range(NEWHOPE_N):
        out[rev(i)] = array[i]
    return out


def iterative_ntt(a):
    """
    Iterative NTT in wikipedia. BO -> NO
    """

    # a = bit_rev(a)
    for s in range(logN):
        m0 = 1 << s
        m1 = 1 << (s + 1)

        # print (NEWHOPE_N//m)
        omega_m = omega_table[NEWHOPE_N >> (s + 1)]

        for k in range(0, NEWHOPE_N, m1):
            omega_tmp = 1
            # index = 0
            for j in range(m0):
                b = j + k
                u = a[b]
                t1 = a[b + m0]
                t = omega_tmp * a[b + m0]

                a[b] = (u + t) % NEWHOPE_Q
                a[b + m0] = (u - t) % NEWHOPE_Q

                if DEBUG and b < less_32:
                    print('{} {} | {} {} | {} {} | "{}"'.format(
                        b, b + m0, u, t1, a[b], a[b + m0],
                        omega_table.index(omega_tmp)))
                # index += omega_m
                omega_tmp = (omega_tmp * omega_m) % NEWHOPE_Q

            # print '-----------------'
        if DEBUG:
            print('=================', s)
    return a


def iterative_ntt_2x2(a):
    """
    Iterative NTT in wikipedia. BO -> NO
    """
    sel_ram1 = []
    sel_ram2 = []

    A, B, C, D = [], [], [], []
    dual_port_rom1 = set()
    dual_port_rom2 = set()

    # a = bit_rev(a)
    for s in range(1, logN, 2):
        m0 = 1 << (s - 1)
        m1 = 1 << s
        m2 = 1 << (s + 1)

        omega_m1 = NEWHOPE_N >> s
        omega_m2 = NEWHOPE_N >> (s + 1)

        _index1 = _index2 = 0
        _index3 = 0
        _index4 = NEWHOPE_N >> 2
        # print omega_m1
        # print omega_m2
        # print('=========================', s)
        for k in range(0, NEWHOPE_N, m2):
            for j in range(m0):
                b = j + k
                
                # print(s, b, b + m0, b+m1, b+m1+m0)

                # NTT1
                # prepare omega_tmp

                # _index1 = (b & (m0-1)) * omega_m1
                # _index2 = ((b+m1) & (m0-1)) * omega_m1

                omega_tmp1 = psis_table[_index1 << 1]
                omega_tmp2 = psis_table[_index2 << 1]

                # FOR HLS PURPOSE
                dual_port_rom1.add(_index1)
                dual_port_rom1.add(_index2)

                u11 = a[b]  # top0
                t11 = a[b + m0]  # top1
                u12 = a[b + m1]  # bot0

                t12 = a[b + m1 + m0]  # bot1

                tt11 = omega_tmp1 * t11
                tt12 = omega_tmp2 * t12

                # a[b], a[b+m//2]
                top_p = (u11 + tt11) % NEWHOPE_Q
                top_m = (u11 - tt11) % NEWHOPE_Q

                # a[b+m], a[b +m +m//2]
                bot_p = (u12 + tt12) % NEWHOPE_Q
                bot_m = (u12 - tt12) % NEWHOPE_Q

                # FOR DEBUG
                assert _index1 == _index2
                pp = '{} {} | {} {} | {} {} | "{}" '.format(
                    b, b + m0, u11, t11, top_p, top_m, _index1)
                pp += '| {} {} | {} {} | {} {}'.format(b + m1, b + m1 + m0,
                                                       u12, t12, bot_p, bot_m)

                sel_ram1.append(_index1)
                sel_ram1.append(_index1)

                #if DEBUG1 and b < less_64:
                # print(pp)

                _index2 = _index1 = (_index1 + omega_m1) & (NEWHOPE_N // 2 - 1)

                # FOR DEBUG

                # SWAP
                # top[1] = b + m = bot[0]
                # bot[0] = b + m//2 = top[1]
                bot_p, top_m = top_m, bot_p

                # NTT2
                # print bin( ((b) & (m1 - 1))  )
                # print bin( ((b+m0) & (m1 - 1))  )
                # print
                _index3 = ((b) & (m1 - 1)) * omega_m2
                _index4 = ((b + m0) & (m1 - 1)) * omega_m2

                sel_ram2.append(_index3)
                sel_ram2.append(_index4)

                dual_port_rom2.add(_index3)
                dual_port_rom2.add(_index4)

                # prepare omega_tmp
                omega_tmp_3 = psis_table[_index3 << 1]
                omega_tmp_4 = psis_table[_index4 << 1]

                u21 = top_p  # u1 + t1
                t21 = top_m  # u2 + t2

                u22 = bot_p  # u1 - t1
                t22 = bot_m  # u2 - t2

                tt21 = omega_tmp_3 * t21
                tt22 = omega_tmp_4 * t22

                out_top_p = (u21 + tt21) % NEWHOPE_Q
                out_top_m = (u21 - tt21) % NEWHOPE_Q

                out_bot_p = (u22 + tt22) % NEWHOPE_Q
                out_bot_m = (u22 - tt22) % NEWHOPE_Q

                a[b] = out_top_p
                a[b + m1] = out_top_m

                a[b + m0] = out_bot_p
                a[b + m1 + m0] = out_bot_m
                A.append(b)
                B.append(b + m1)
                C.append(b + m0)
                D.append(b + m1 + m0)

                # FOR DEBUG

                pp = '{} {} | {} {} | {} {} | "{}"\n'.format(
                    b, b + m1, u21, t21, a[b], a[b + m1], _index3)
                pp += '{} {} | {} {} | {} {} | "{}"\n'.format(
                    b + m0, b + m1 + m0, u22, t22, a[b + m0], a[b + m1 + m0],
                    _index4)

                #if DEBUG1 and b < less_64:
                # print(pp)
                # FOR DEBUG
                _index3 = (_index3 + omega_m2) & 0x1ff
                _index4 = (_index4 + omega_m2) & 0x1ff

        if DEBUG1:
            print('===================="', s, omega_m1, omega_m2)

    if DEBUG1:
        # print len((dual_port_rom1)), len((dual_port_rom2))
        # print dual_port_rom1
        # print dual_port_rom2
        # print '='*20,
        # print sorted(A), len(A), len(set(A))
        # print sorted(B), len(B), len(set(B))
        # print sorted(C), len(C), len(set(C))
        # print sorted(D), len(D), len(set(D))
        pass

    # print(sel_ram1)
    # print(sel_ram2)

    return a


def iterative_ntt_ches(a):
    """
    BO -> NO 
    Iteratite NTT in CHES paper
    https:////practice-project.eu//downloads//publications//ches_final.pdf
    """
    # a = bit_rev(a)

    # for s in range(logN):
    for s in range(logN):
        m = 2 << s
        omega_m = NEWHOPE_N // m
        omega_tmp = 0

        for j in range(m // 2):
            for k in range(0, NEWHOPE_N, m):
                b = k + j
                # print(b, b + m//2, omega_tmp)

                u = a[b]
                t = a[b + m // 2]

                t = (omega_table[omega_tmp] * t)

                a[b] = (u + t) % NEWHOPE_Q
                a[b + m // 2] = (u - t) % NEWHOPE_Q

                if DEBUG2 and b < less_32:
                    print(
                        '{} {} | {} {} | {} {} | "{}"'.format(
                            b, b + m // 2, u, t, a[b], a[b + m // 2],
                            omega_tmp), '--', s)

            omega_tmp += omega_m
        # print('=================', s)
        if DEBUG2:
            pass
    return a


def iterative_ntt_2x2_ches(a):
    """
    BO -> NO 
    """
    sel_ram1 = []
    sel_ram2 = []

    A, B, C, D = [], [], [], []
    dual_port_rom1 = set()
    dual_port_rom2 = set()

    # For printing index purpose
    ram1, ram2, ram3, ram4 = [], [], [], []

    # a = bit_rev(a)

    for s in range(1, logN, 2):
        m0 = 1 << (s - 1)
        m1 = 1 << (s)
        m2 = 1 << (s + 1)
        omega_m1 = NEWHOPE_N >> s
        omega_m2 = NEWHOPE_N >> (s + 1)

        assert m0 == m1 >> 1
        assert m1 == m2 >> 1

        _index1 = _index2 = 0
        _index3 = 0
        _index4 = NEWHOPE_N >> 2
        print('=========================', s)
        for j in range(m0):
            for k in range(0, NEWHOPE_N, m2):
                b = k + j
                print(s, b, b + m0, b+m1, b+m1+m0)

                # NTT1
                # prepare omega_tmp

                # _index1 = (b & (m0-1)) * omega_m1
                # _index2 = ((b+m1) & (m0-1)) * omega_m1

                omega_tmp1 = omega_table[_index1]
                omega_tmp2 = omega_table[_index2]

                sel_ram1.append(_index1)
                sel_ram1.append(_index2)

                # FOR HLS PURPOSE
                dual_port_rom1.add(_index1)
                dual_port_rom1.add(_index2)

                u11 = a[b]  # top0
                t11 = a[b + m0]  # top1
                u12 = a[b + m1]  # bot0
                t12 = a[b + m1 + m0]  # bot1

                tt11 = omega_tmp1 * t11
                tt12 = omega_tmp2 * t12

                # a[b], a[b+m//2]
                top_p = (u11 + tt11) % NEWHOPE_Q
                top_m = (u11 - tt11) % NEWHOPE_Q

                # a[b+m], a[b +m +m//2]
                bot_p = (u12 + tt12) % NEWHOPE_Q
                bot_m = (u12 - tt12) % NEWHOPE_Q

                # FOR DEBUG
                assert _index1 == _index2
                pp = '{} {} | {} {} | {} {} | "{}" '.format(
                    b, b + m0, u11, t11, top_p, top_m, _index1)
                pp += '| {} {} | {} {} | {} {}'.format(b + m1, b + m1 + m0,
                                                       u12, t12, bot_p, bot_m)

                if DEBUG3 and b < less_64:
                    # print b, j, k
                    print(pp)

                # FOR DEBUG

                # SWAP
                # top[1] = b + m = bot[0]
                # bot[0] = b + m//2 = top[1]
                bot_p, top_m = top_m, bot_p

                # NTT2

                # _index3 = ((b) & (m1 - 1)) * omega_m2
                # _index4 = ((b+m0) & (m1 - 1)) * omega_m2

                dual_port_rom2.add(_index3)
                dual_port_rom2.add(_index4)

                # prepare omega_tmp
                omega_tmp_3 = omega_table[_index3]
                omega_tmp_4 = omega_table[_index4]

                sel_ram2.append(_index3)
                sel_ram2.append(_index4)

                u21 = top_p  # u1 + t1
                t21 = top_m  # u2 + t2

                u22 = bot_p  # u1 - t1
                t22 = bot_m  # u2 - t2

                tt21 = omega_tmp_3 * t21
                tt22 = omega_tmp_4 * t22

                out_top_p = (u21 + tt21) % NEWHOPE_Q
                out_top_m = (u21 - tt21) % NEWHOPE_Q

                out_bot_p = (u22 + tt22) % NEWHOPE_Q
                out_bot_m = (u22 - tt22) % NEWHOPE_Q

                a[b] = out_top_p
                a[b + m1] = out_top_m

                a[b + m0] = out_bot_p
                a[b + m1 + m0] = out_bot_m
                A.append(b)
                B.append(b + m1)
                C.append(b + m0)
                D.append(b + m1 + m0)

                # FOR DEBUG

                pp = '{} {} | {} {} | {} {} | "{}"\n'.format(
                    b, b + m1, u21, t21, a[b], a[b + m1], _index3)
                pp += '{} {} | {} {} | {} {} | "{}"\n'.format(
                    b + m0, b + m1 + m0, u22, t22, a[b + m0], a[b + m1 + m0],
                    _index4)

                if DEBUG3 and b < less_64:
                    print(pp)
                # FOR DEBUG

                if s == 9:
                    ram1.append(a[b])
                    ram2.append(a[b + m1])
                    ram3.append(a[b + m0])
                    ram4.append(a[b + m1 + m0])

            _index1 += omega_m1
            _index2 = _index1
            _index3 += omega_m2
            _index4 += omega_m2
        if DEBUG3:
            print('===================="', s, omega_m1, omega_m2)

    if DEBUG3:
        # print len((dual_port_rom1)), len((dual_port_rom2))
        # print dual_port_rom1
        # print dual_port_rom2
        # print '='*20,
        # print sorted(A), len(A), len(set(A))
        # print sorted(B), len(B), len(set(B))
        # print sorted(C), len(C), len(set(C))
        # print sorted(D), len(D), len(set(D))
        pass

    topram = [ram1, ram2, ram3, ram4]

    # print("RAM1", sel_ram1)
    # print("RAM2", sel_ram2)

    return a, topram


if __name__ == "__main__":
    # Generate data
    data_b = copy.copy(data_a)
    data_c = copy.copy(data_a)
    data_d = copy.copy(data_a)
    origin_a = copy.copy(data_a)

    ntt_b = iterative_ntt_ches(data_b)
    ntt_c, _ = iterative_ntt_2x2_ches(data_c)

    assert ntt_b == ntt_c
    print("PASSED: iterative_ntt_ches == iterative_ntt_2x2_ches")

    ntt_d = iterative_ntt(data_d)
    ntt_a = iterative_ntt_2x2(data_a)

    assert ntt_a == ntt_d

    print("PASSED: iterative_ntt == iterative_ntt_2x2")

    assert ntt_a == ntt_b == ntt_c == ntt_d != origin_a
    print("PASSED: iterative_ntt_ches == iterative_ntt_2x2_ches == iterative_ntt == iterative_ntt_2x2")
