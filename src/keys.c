/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   keys.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aezzeddi <aezzeddi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/09/27 17:28:55 by aezzeddi          #+#    #+#             */
/*   Updated: 2017/09/29 02:31:23 by aezzeddi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ssl.h"

extern t_options g_options;

const int g_pc1[56] = {
	57, 49, 41, 33, 25, 17, 9,
	1, 58, 50, 42, 34, 26, 18,
	10, 2, 59, 51, 43, 35, 27,
	19, 11, 3, 60, 52, 44, 36,
	63, 55, 47, 39, 31, 23, 15,
	7, 62, 54, 46, 38, 30, 22,
	14, 6, 61, 53, 45, 37, 29,
	21, 13, 5, 28, 20, 12, 4
};

const int g_rotations[16] = {
	1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1
};

const int g_pc2[48] = {
	14, 17, 11, 24, 1, 5,
	3, 28, 15, 6, 21, 10,
	23, 19, 12, 4, 26, 8,
	16, 7, 27, 20, 13, 2,
	41, 52, 31, 37, 47, 55,
	30, 40, 51, 45, 33, 48,
	44, 49, 39, 56, 34, 53,
	46, 42, 50, 36, 29, 32
};

char	*normalize_key(char *old_key, int key_size)
{
	char	*new_key;
	int		len;
	int		i;

	i = 1;
	len = ft_strlen(old_key);
	if (!len)
		return (ft_strnew(key_size));
	new_key = old_key;
	if (len < key_size)
	{
		new_key = ft_strdup(old_key);
		while (i < key_size / len)
		{
			new_key = ft_strfjoin(new_key, old_key);
			i++;
		}
		new_key = ft_strfjoin(new_key, ft_strsub(old_key, 0, key_size % len));
	}
	return (new_key);
}

t_ulong	rotate_sub_key(t_ulong key, int shift)
{
	t_ulong	left_half;
	t_ulong	right_half;

	left_half = (key >> (0x40 - 28)) << (0x40 - 28);
	right_half = key << 28;
	left_half = (left_half << shift) ^ ((left_half >> (64 - shift)) << 36);
	right_half = (right_half << shift) ^ ((right_half >> (64 - shift)) << 36);
	return (left_half ^ (right_half >> 28));
}

t_ulong	*create_key_schedule(char *key)
{
	t_ulong	k;
	t_ulong	*sub_keys;
	int		i;

	sub_keys = malloc(16 * sizeof(t_ulong));
	k = permute(convert_hex_to_bits(key), g_pc1, 56);
	i = 0;
	while (i < 16)
	{
		k = rotate_sub_key(k, g_rotations[i]);
		sub_keys[i] = permute(k, g_pc2, 48);
		i++;
	}
	return (sub_keys);
}

t_ulong	**create_3_key_schedule(char *key_192)
{
	t_ulong	**key_schedules;

	key_schedules = malloc(3 * sizeof(t_ulong *));
	key_schedules[0] =
		create_key_schedule(g_options.mode == Encrypt ? key_192 : key_192 + 32);
	key_schedules[1] = create_key_schedule(key_192 + 16);
	key_schedules[2] =
		create_key_schedule(g_options.mode == Encrypt ? key_192 + 32 : key_192);
	return (key_schedules);
}
