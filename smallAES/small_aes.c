/* small_aes.c
 *
 * Copyright (c) 2012 Rafael Azuero Hurtado - German Londoño Paez
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. 
 */

#include "small_aes.h"

static const unsigned int rcon[] = {
    0x01000000, 0x02000000, 0x04000000, 0x08000000,
    0x10000000, 0x20000000, 0x40000000, 0x80000000,
    0x1B000000, 0x36000000,     
};


static const unsigned int Te[256] = {
    0xc66363a5U, 0xf87c7c84U, 0xee777799U, 0xf67b7b8dU,
    0xfff2f20dU, 0xd66b6bbdU, 0xde6f6fb1U, 0x91c5c554U,
    0x60303050U, 0x02010103U, 0xce6767a9U, 0x562b2b7dU,
    0xe7fefe19U, 0xb5d7d762U, 0x4dababe6U, 0xec76769aU,
    0x8fcaca45U, 0x1f82829dU, 0x89c9c940U, 0xfa7d7d87U,
    0xeffafa15U, 0xb25959ebU, 0x8e4747c9U, 0xfbf0f00bU,
    0x41adadecU, 0xb3d4d467U, 0x5fa2a2fdU, 0x45afafeaU,
    0x239c9cbfU, 0x53a4a4f7U, 0xe4727296U, 0x9bc0c05bU,
    0x75b7b7c2U, 0xe1fdfd1cU, 0x3d9393aeU, 0x4c26266aU,
    0x6c36365aU, 0x7e3f3f41U, 0xf5f7f702U, 0x83cccc4fU,
    0x6834345cU, 0x51a5a5f4U, 0xd1e5e534U, 0xf9f1f108U,
    0xe2717193U, 0xabd8d873U, 0x62313153U, 0x2a15153fU,
    0x0804040cU, 0x95c7c752U, 0x46232365U, 0x9dc3c35eU,
    0x30181828U, 0x379696a1U, 0x0a05050fU, 0x2f9a9ab5U,
    0x0e070709U, 0x24121236U, 0x1b80809bU, 0xdfe2e23dU,
    0xcdebeb26U, 0x4e272769U, 0x7fb2b2cdU, 0xea75759fU,
    0x1209091bU, 0x1d83839eU, 0x582c2c74U, 0x341a1a2eU,
    0x361b1b2dU, 0xdc6e6eb2U, 0xb45a5aeeU, 0x5ba0a0fbU,
    0xa45252f6U, 0x763b3b4dU, 0xb7d6d661U, 0x7db3b3ceU,
    0x5229297bU, 0xdde3e33eU, 0x5e2f2f71U, 0x13848497U,
    0xa65353f5U, 0xb9d1d168U, 0x00000000U, 0xc1eded2cU,
    0x40202060U, 0xe3fcfc1fU, 0x79b1b1c8U, 0xb65b5bedU,
    0xd46a6abeU, 0x8dcbcb46U, 0x67bebed9U, 0x7239394bU,
    0x944a4adeU, 0x984c4cd4U, 0xb05858e8U, 0x85cfcf4aU,
    0xbbd0d06bU, 0xc5efef2aU, 0x4faaaae5U, 0xedfbfb16U,
    0x864343c5U, 0x9a4d4dd7U, 0x66333355U, 0x11858594U,
    0x8a4545cfU, 0xe9f9f910U, 0x04020206U, 0xfe7f7f81U,
    0xa05050f0U, 0x783c3c44U, 0x259f9fbaU, 0x4ba8a8e3U,
    0xa25151f3U, 0x5da3a3feU, 0x804040c0U, 0x058f8f8aU,
    0x3f9292adU, 0x219d9dbcU, 0x70383848U, 0xf1f5f504U,
    0x63bcbcdfU, 0x77b6b6c1U, 0xafdada75U, 0x42212163U,
    0x20101030U, 0xe5ffff1aU, 0xfdf3f30eU, 0xbfd2d26dU,
    0x81cdcd4cU, 0x180c0c14U, 0x26131335U, 0xc3ecec2fU,
    0xbe5f5fe1U, 0x359797a2U, 0x884444ccU, 0x2e171739U,
    0x93c4c457U, 0x55a7a7f2U, 0xfc7e7e82U, 0x7a3d3d47U,
    0xc86464acU, 0xba5d5de7U, 0x3219192bU, 0xe6737395U,
    0xc06060a0U, 0x19818198U, 0x9e4f4fd1U, 0xa3dcdc7fU,
    0x44222266U, 0x542a2a7eU, 0x3b9090abU, 0x0b888883U,
    0x8c4646caU, 0xc7eeee29U, 0x6bb8b8d3U, 0x2814143cU,
    0xa7dede79U, 0xbc5e5ee2U, 0x160b0b1dU, 0xaddbdb76U,
    0xdbe0e03bU, 0x64323256U, 0x743a3a4eU, 0x140a0a1eU,
    0x924949dbU, 0x0c06060aU, 0x4824246cU, 0xb85c5ce4U,
    0x9fc2c25dU, 0xbdd3d36eU, 0x43acacefU, 0xc46262a6U,
    0x399191a8U, 0x319595a4U, 0xd3e4e437U, 0xf279798bU,
    0xd5e7e732U, 0x8bc8c843U, 0x6e373759U, 0xda6d6db7U,
    0x018d8d8cU, 0xb1d5d564U, 0x9c4e4ed2U, 0x49a9a9e0U,
    0xd86c6cb4U, 0xac5656faU, 0xf3f4f407U, 0xcfeaea25U,
    0xca6565afU, 0xf47a7a8eU, 0x47aeaee9U, 0x10080818U,
    0x6fbabad5U, 0xf0787888U, 0x4a25256fU, 0x5c2e2e72U,
    0x381c1c24U, 0x57a6a6f1U, 0x73b4b4c7U, 0x97c6c651U,
    0xcbe8e823U, 0xa1dddd7cU, 0xe874749cU, 0x3e1f1f21U,
    0x964b4bddU, 0x61bdbddcU, 0x0d8b8b86U, 0x0f8a8a85U,
    0xe0707090U, 0x7c3e3e42U, 0x71b5b5c4U, 0xcc6666aaU,
    0x904848d8U, 0x06030305U, 0xf7f6f601U, 0x1c0e0e12U,
    0xc26161a3U, 0x6a35355fU, 0xae5757f9U, 0x69b9b9d0U,
    0x17868691U, 0x99c1c158U, 0x3a1d1d27U, 0x279e9eb9U,
    0xd9e1e138U, 0xebf8f813U, 0x2b9898b3U, 0x22111133U,
    0xd26969bbU, 0xa9d9d970U, 0x078e8e89U, 0x339494a7U,
    0x2d9b9bb6U, 0x3c1e1e22U, 0x15878792U, 0xc9e9e920U,
    0x87cece49U, 0xaa5555ffU, 0x50282878U, 0xa5dfdf7aU,
    0x038c8c8fU, 0x59a1a1f8U, 0x09898980U, 0x1a0d0d17U,
    0x65bfbfdaU, 0xd7e6e631U, 0x844242c6U, 0xd06868b8U,
    0x824141c3U, 0x299999b0U, 0x5a2d2d77U, 0x1e0f0f11U,
    0x7bb0b0cbU, 0xa85454fcU, 0x6dbbbbd6U, 0x2c16163aU,
};

static const unsigned int Td[2][256] = {
{
    0x51f4a750U, 0x7e416553U, 0x1a17a4c3U, 0x3a275e96U,
    0x3bab6bcbU, 0x1f9d45f1U, 0xacfa58abU, 0x4be30393U,
    0x2030fa55U, 0xad766df6U, 0x88cc7691U, 0xf5024c25U,
    0x4fe5d7fcU, 0xc52acbd7U, 0x26354480U, 0xb562a38fU,
    0xdeb15a49U, 0x25ba1b67U, 0x45ea0e98U, 0x5dfec0e1U,
    0xc32f7502U, 0x814cf012U, 0x8d4697a3U, 0x6bd3f9c6U,
    0x038f5fe7U, 0x15929c95U, 0xbf6d7aebU, 0x955259daU,
    0xd4be832dU, 0x587421d3U, 0x49e06929U, 0x8ec9c844U,
    0x75c2896aU, 0xf48e7978U, 0x99583e6bU, 0x27b971ddU,
    0xbee14fb6U, 0xf088ad17U, 0xc920ac66U, 0x7dce3ab4U,
    0x63df4a18U, 0xe51a3182U, 0x97513360U, 0x62537f45U,
    0xb16477e0U, 0xbb6bae84U, 0xfe81a01cU, 0xf9082b94U,
    0x70486858U, 0x8f45fd19U, 0x94de6c87U, 0x527bf8b7U,
    0xab73d323U, 0x724b02e2U, 0xe31f8f57U, 0x6655ab2aU,
    0xb2eb2807U, 0x2fb5c203U, 0x86c57b9aU, 0xd33708a5U,
    0x302887f2U, 0x23bfa5b2U, 0x02036abaU, 0xed16825cU,
    0x8acf1c2bU, 0xa779b492U, 0xf307f2f0U, 0x4e69e2a1U,
    0x65daf4cdU, 0x0605bed5U, 0xd134621fU, 0xc4a6fe8aU,
    0x342e539dU, 0xa2f355a0U, 0x058ae132U, 0xa4f6eb75U,
    0x0b83ec39U, 0x4060efaaU, 0x5e719f06U, 0xbd6e1051U,
    0x3e218af9U, 0x96dd063dU, 0xdd3e05aeU, 0x4de6bd46U,
    0x91548db5U, 0x71c45d05U, 0x0406d46fU, 0x605015ffU,
    0x1998fb24U, 0xd6bde997U, 0x894043ccU, 0x67d99e77U,
    0xb0e842bdU, 0x07898b88U, 0xe7195b38U, 0x79c8eedbU,
    0xa17c0a47U, 0x7c420fe9U, 0xf8841ec9U, 0x00000000U,
    0x09808683U, 0x322bed48U, 0x1e1170acU, 0x6c5a724eU,
    0xfd0efffbU, 0x0f853856U, 0x3daed51eU, 0x362d3927U,
    0x0a0fd964U, 0x685ca621U, 0x9b5b54d1U, 0x24362e3aU,
    0x0c0a67b1U, 0x9357e70fU, 0xb4ee96d2U, 0x1b9b919eU,
    0x80c0c54fU, 0x61dc20a2U, 0x5a774b69U, 0x1c121a16U,
    0xe293ba0aU, 0xc0a02ae5U, 0x3c22e043U, 0x121b171dU,
    0x0e090d0bU, 0xf28bc7adU, 0x2db6a8b9U, 0x141ea9c8U,
    0x57f11985U, 0xaf75074cU, 0xee99ddbbU, 0xa37f60fdU,
    0xf701269fU, 0x5c72f5bcU, 0x44663bc5U, 0x5bfb7e34U,
    0x8b432976U, 0xcb23c6dcU, 0xb6edfc68U, 0xb8e4f163U,
    0xd731dccaU, 0x42638510U, 0x13972240U, 0x84c61120U,
    0x854a247dU, 0xd2bb3df8U, 0xaef93211U, 0xc729a16dU,
    0x1d9e2f4bU, 0xdcb230f3U, 0x0d8652ecU, 0x77c1e3d0U,
    0x2bb3166cU, 0xa970b999U, 0x119448faU, 0x47e96422U,
    0xa8fc8cc4U, 0xa0f03f1aU, 0x567d2cd8U, 0x223390efU,
    0x87494ec7U, 0xd938d1c1U, 0x8ccaa2feU, 0x98d40b36U,
    0xa6f581cfU, 0xa57ade28U, 0xdab78e26U, 0x3fadbfa4U,
    0x2c3a9de4U, 0x5078920dU, 0x6a5fcc9bU, 0x547e4662U,
    0xf68d13c2U, 0x90d8b8e8U, 0x2e39f75eU, 0x82c3aff5U,
    0x9f5d80beU, 0x69d0937cU, 0x6fd52da9U, 0xcf2512b3U,
    0xc8ac993bU, 0x10187da7U, 0xe89c636eU, 0xdb3bbb7bU,
    0xcd267809U, 0x6e5918f4U, 0xec9ab701U, 0x834f9aa8U,
    0xe6956e65U, 0xaaffe67eU, 0x21bccf08U, 0xef15e8e6U,
    0xbae79bd9U, 0x4a6f36ceU, 0xea9f09d4U, 0x29b07cd6U,
    0x31a4b2afU, 0x2a3f2331U, 0xc6a59430U, 0x35a266c0U,
    0x744ebc37U, 0xfc82caa6U, 0xe090d0b0U, 0x33a7d815U,
    0xf104984aU, 0x41ecdaf7U, 0x7fcd500eU, 0x1791f62fU,
    0x764dd68dU, 0x43efb04dU, 0xccaa4d54U, 0xe49604dfU,
    0x9ed1b5e3U, 0x4c6a881bU, 0xc12c1fb8U, 0x4665517fU,
    0x9d5eea04U, 0x018c355dU, 0xfa877473U, 0xfb0b412eU,
    0xb3671d5aU, 0x92dbd252U, 0xe9105633U, 0x6dd64713U,
    0x9ad7618cU, 0x37a10c7aU, 0x59f8148eU, 0xeb133c89U,
    0xcea927eeU, 0xb761c935U, 0xe11ce5edU, 0x7a47b13cU,
    0x9cd2df59U, 0x55f2733fU, 0x1814ce79U, 0x73c737bfU,
    0x53f7cdeaU, 0x5ffdaa5bU, 0xdf3d6f14U, 0x7844db86U,
    0xcaaff381U, 0xb968c43eU, 0x3824342cU, 0xc2a3405fU,
    0x161dc372U, 0xbce2250cU, 0x283c498bU, 0xff0d9541U,
    0x39a80171U, 0x080cb3deU, 0xd8b4e49cU, 0x6456c190U,
    0x7bcb8461U, 0xd532b670U, 0x486c5c74U, 0xd0b85742U,
},
{
    0x52525252U, 0x09090909U, 0x6a6a6a6aU, 0xd5d5d5d5U,
    0x30303030U, 0x36363636U, 0xa5a5a5a5U, 0x38383838U,
    0xbfbfbfbfU, 0x40404040U, 0xa3a3a3a3U, 0x9e9e9e9eU,
    0x81818181U, 0xf3f3f3f3U, 0xd7d7d7d7U, 0xfbfbfbfbU,
    0x7c7c7c7cU, 0xe3e3e3e3U, 0x39393939U, 0x82828282U,
    0x9b9b9b9bU, 0x2f2f2f2fU, 0xffffffffU, 0x87878787U,
    0x34343434U, 0x8e8e8e8eU, 0x43434343U, 0x44444444U,
    0xc4c4c4c4U, 0xdedededeU, 0xe9e9e9e9U, 0xcbcbcbcbU,
    0x54545454U, 0x7b7b7b7bU, 0x94949494U, 0x32323232U,
    0xa6a6a6a6U, 0xc2c2c2c2U, 0x23232323U, 0x3d3d3d3dU,
    0xeeeeeeeeU, 0x4c4c4c4cU, 0x95959595U, 0x0b0b0b0bU,
    0x42424242U, 0xfafafafaU, 0xc3c3c3c3U, 0x4e4e4e4eU,
    0x08080808U, 0x2e2e2e2eU, 0xa1a1a1a1U, 0x66666666U,
    0x28282828U, 0xd9d9d9d9U, 0x24242424U, 0xb2b2b2b2U,
    0x76767676U, 0x5b5b5b5bU, 0xa2a2a2a2U, 0x49494949U,
    0x6d6d6d6dU, 0x8b8b8b8bU, 0xd1d1d1d1U, 0x25252525U,
    0x72727272U, 0xf8f8f8f8U, 0xf6f6f6f6U, 0x64646464U,
    0x86868686U, 0x68686868U, 0x98989898U, 0x16161616U,
    0xd4d4d4d4U, 0xa4a4a4a4U, 0x5c5c5c5cU, 0xccccccccU,
    0x5d5d5d5dU, 0x65656565U, 0xb6b6b6b6U, 0x92929292U,
    0x6c6c6c6cU, 0x70707070U, 0x48484848U, 0x50505050U,
    0xfdfdfdfdU, 0xededededU, 0xb9b9b9b9U, 0xdadadadaU,
    0x5e5e5e5eU, 0x15151515U, 0x46464646U, 0x57575757U,
    0xa7a7a7a7U, 0x8d8d8d8dU, 0x9d9d9d9dU, 0x84848484U,
    0x90909090U, 0xd8d8d8d8U, 0xababababU, 0x00000000U,
    0x8c8c8c8cU, 0xbcbcbcbcU, 0xd3d3d3d3U, 0x0a0a0a0aU,
    0xf7f7f7f7U, 0xe4e4e4e4U, 0x58585858U, 0x05050505U,
    0xb8b8b8b8U, 0xb3b3b3b3U, 0x45454545U, 0x06060606U,
    0xd0d0d0d0U, 0x2c2c2c2cU, 0x1e1e1e1eU, 0x8f8f8f8fU,
    0xcacacacaU, 0x3f3f3f3fU, 0x0f0f0f0fU, 0x02020202U,
    0xc1c1c1c1U, 0xafafafafU, 0xbdbdbdbdU, 0x03030303U,
    0x01010101U, 0x13131313U, 0x8a8a8a8aU, 0x6b6b6b6bU,
    0x3a3a3a3aU, 0x91919191U, 0x11111111U, 0x41414141U,
    0x4f4f4f4fU, 0x67676767U, 0xdcdcdcdcU, 0xeaeaeaeaU,
    0x97979797U, 0xf2f2f2f2U, 0xcfcfcfcfU, 0xcecececeU,
    0xf0f0f0f0U, 0xb4b4b4b4U, 0xe6e6e6e6U, 0x73737373U,
    0x96969696U, 0xacacacacU, 0x74747474U, 0x22222222U,
    0xe7e7e7e7U, 0xadadadadU, 0x35353535U, 0x85858585U,
    0xe2e2e2e2U, 0xf9f9f9f9U, 0x37373737U, 0xe8e8e8e8U,
    0x1c1c1c1cU, 0x75757575U, 0xdfdfdfdfU, 0x6e6e6e6eU,
    0x47474747U, 0xf1f1f1f1U, 0x1a1a1a1aU, 0x71717171U,
    0x1d1d1d1dU, 0x29292929U, 0xc5c5c5c5U, 0x89898989U,
    0x6f6f6f6fU, 0xb7b7b7b7U, 0x62626262U, 0x0e0e0e0eU,
    0xaaaaaaaaU, 0x18181818U, 0xbebebebeU, 0x1b1b1b1bU,
    0xfcfcfcfcU, 0x56565656U, 0x3e3e3e3eU, 0x4b4b4b4bU,
    0xc6c6c6c6U, 0xd2d2d2d2U, 0x79797979U, 0x20202020U,
    0x9a9a9a9aU, 0xdbdbdbdbU, 0xc0c0c0c0U, 0xfefefefeU,
    0x78787878U, 0xcdcdcdcdU, 0x5a5a5a5aU, 0xf4f4f4f4U,
    0x1f1f1f1fU, 0xddddddddU, 0xa8a8a8a8U, 0x33333333U,
    0x88888888U, 0x07070707U, 0xc7c7c7c7U, 0x31313131U,
    0xb1b1b1b1U, 0x12121212U, 0x10101010U, 0x59595959U,
    0x27272727U, 0x80808080U, 0xececececU, 0x5f5f5f5fU,
    0x60606060U, 0x51515151U, 0x7f7f7f7fU, 0xa9a9a9a9U,
    0x19191919U, 0xb5b5b5b5U, 0x4a4a4a4aU, 0x0d0d0d0dU,
    0x2d2d2d2dU, 0xe5e5e5e5U, 0x7a7a7a7aU, 0x9f9f9f9fU,
    0x93939393U, 0xc9c9c9c9U, 0x9c9c9c9cU, 0xefefefefU,
    0xa0a0a0a0U, 0xe0e0e0e0U, 0x3b3b3b3bU, 0x4d4d4d4dU,
    0xaeaeaeaeU, 0x2a2a2a2aU, 0xf5f5f5f5U, 0xb0b0b0b0U,
    0xc8c8c8c8U, 0xebebebebU, 0xbbbbbbbbU, 0x3c3c3c3cU,
    0x83838383U, 0x53535353U, 0x99999999U, 0x61616161U,
    0x17171717U, 0x2b2b2b2bU, 0x04040404U, 0x7e7e7e7eU,
    0xbabababaU, 0x77777777U, 0xd6d6d6d6U, 0x26262626U,
    0xe1e1e1e1U, 0x69696969U, 0x14141414U, 0x63636363U,
    0x55555555U, 0x21212121U, 0x0c0c0c0cU, 0x7d7d7d7dU,
}
};

#define GETBYTE(x, y) (unsigned int)((unsigned char)((x) >> (8 * (y))))
#define ROR(x, y) (unsigned int) (rotrFixed((x), 8 * (y)))
#define ROL(x, y) (unsigned int) (rotlFixed((x), 8 * (y)))

int aesSetKey(AES* aes, const unsigned char* userKey, unsigned int keylen, int dir) {
    unsigned int temp, *rk = aes->key;
    unsigned int i = 0;

    if (!((keylen == 16) || (keylen == 24) || (keylen == 32)))
        return -1;

    aes->rounds = keylen/4 + 6;

    memcpy(rk, userKey, keylen);
    byteReverseWords(rk, rk, keylen);
        
    switch(keylen)
    {
    case 16:
        while (1)
        {
            temp  = rk[3];
            rk[4] = rk[0] ^
                (Te[GETBYTE(temp, 2)] & 0x00ff0000) << 8 ^
                (Te[GETBYTE(temp, 1)] & 0x00ff0000) ^
                (Te[GETBYTE(temp, 0)] & 0x0000ff00) ^
                (Te[GETBYTE(temp, 3)] & 0x0000ff00) >> 8 ^
                rcon[i];
            rk[5] = rk[1] ^ rk[4];
            rk[6] = rk[2] ^ rk[5];
            rk[7] = rk[3] ^ rk[6];            
            if (++i == 10)
                break;
            rk += 4;
        }
        break;

    case 24:
        while (1)
        {
            temp = rk[ 5];
            rk[ 6] = rk[ 0] ^
                (Te[GETBYTE(temp, 2)] & 0x00ff0000) << 8  ^
                (Te[GETBYTE(temp, 1)] & 0x00ff0000) ^
                (Te[GETBYTE(temp, 0)] & 0x0000ff00) ^
                (Te[GETBYTE(temp, 3)] & 0x0000ff00) >> 8 ^
                rcon[i];
            rk[ 7] = rk[ 1] ^ rk[ 6];
            rk[ 8] = rk[ 2] ^ rk[ 7];
            rk[ 9] = rk[ 3] ^ rk[ 8];            
            if (++i == 8)
                break;
            rk[10] = rk[ 4] ^ rk[ 9];
            rk[11] = rk[ 5] ^ rk[10];            
            rk += 6;            
        }
        break;

    case 32:
        while (1)
        {
            temp = rk[ 7];
            rk[ 8] = rk[ 0] ^
                (Te[GETBYTE(temp, 2)] & 0x00ff0000) << 8  ^
                (Te[GETBYTE(temp, 1)] & 0x00ff0000) ^
                (Te[GETBYTE(temp, 0)] & 0x0000ff00) ^
                (Te[GETBYTE(temp, 3)] & 0x0000ff00) >> 8 ^
                rcon[i];
            rk[ 9] = rk[ 1] ^ rk[ 8];
            rk[10] = rk[ 2] ^ rk[ 9];
            rk[11] = rk[ 3] ^ rk[10];            
            if (++i == 7)
                break;
            temp = rk[11];
            rk[12] = rk[ 4] ^
                (Te[GETBYTE(temp, 3)] & 0x00ff0000) << 8  ^
                (Te[GETBYTE(temp, 2)] & 0x00ff0000) ^
                (Te[GETBYTE(temp, 1)] & 0x0000ff00) ^
                (Te[GETBYTE(temp, 0)] & 0x0000ff00) >> 8;
            rk[13] = rk[ 5] ^ rk[12];
            rk[14] = rk[ 6] ^ rk[13];
            rk[15] = rk[ 7] ^ rk[14];            
            rk += 8;
        }
        break;
    }

    if (dir == SMALL_AES_DECRYPTION)
    {
        unsigned int i, j;
        rk = aes->key;

        /* invert the order of the round keys: */
        for (i = 0, j = 4* aes->rounds; i < j; i += 4, j -= 4) {
            temp = rk[i    ]; rk[i    ] = rk[j    ]; rk[j    ] = temp;
            temp = rk[i + 1]; rk[i + 1] = rk[j + 1]; rk[j + 1] = temp;
            temp = rk[i + 2]; rk[i + 2] = rk[j + 2]; rk[j + 2] = temp;
            temp = rk[i + 3]; rk[i + 3] = rk[j + 3]; rk[j + 3] = temp;
        }
        /* apply the inverse MixColumn transform to all round keys but the
           first and the last: */
        for (i = 1; i < aes->rounds; i++) {
            rk += 4;
            rk[0] =
                Td[0][GETBYTE(Te[GETBYTE(rk[0], 3)], 2)] ^
                ROR(Td[0][GETBYTE(Te[GETBYTE(rk[0], 2)], 2)], 1) ^
                ROR(Td[0][GETBYTE(Te[GETBYTE(rk[0], 1)], 2)], 2) ^
                ROL(Td[0][GETBYTE(Te[GETBYTE(rk[0], 0)], 2)], 1);
            rk[1] =
                Td[0][GETBYTE(Te[GETBYTE(rk[1], 3)], 2)] ^
                ROR(Td[0][GETBYTE(Te[GETBYTE(rk[1], 2)], 2)], 1) ^
                ROR(Td[0][GETBYTE(Te[GETBYTE(rk[1], 1)], 2)], 2) ^
                ROL(Td[0][GETBYTE(Te[GETBYTE(rk[1], 0)], 2)], 1);
            rk[2] =
                Td[0][GETBYTE(Te[GETBYTE(rk[2], 3)], 2)] ^
                ROR(Td[0][GETBYTE(Te[GETBYTE(rk[2], 2)], 2)], 1) ^
                ROR(Td[0][GETBYTE(Te[GETBYTE(rk[2], 1)], 2)], 2) ^
                ROL(Td[0][GETBYTE(Te[GETBYTE(rk[2], 0)], 2)], 1);
            rk[3] =
                Td[0][GETBYTE(Te[GETBYTE(rk[3], 3)], 2)] ^
                ROR(Td[0][GETBYTE(Te[GETBYTE(rk[3], 2)], 2)], 1) ^
                ROR(Td[0][GETBYTE(Te[GETBYTE(rk[3], 1)], 2)], 2) ^
                ROL(Td[0][GETBYTE(Te[GETBYTE(rk[3], 0)], 2)], 1);                
        }
    }
    
    return 0;
}

void aesEncrypt(AES* aes, const unsigned char *inBlock, unsigned char *outBlock) {
    unsigned int s0, s1, s2, s3;
    unsigned int t0, t1, t2, t3;
    unsigned int r = aes->rounds >> 1;

    const unsigned int* rk = aes->key;
    /*
     * map byte array block to cipher state
     * and add initial round key:
     */
    memcpy(&s0, inBlock,                  sizeof(s0));
    memcpy(&s1, inBlock + sizeof(s0),     sizeof(s1));
    memcpy(&s2, inBlock + 2 * sizeof(s0), sizeof(s2));
    memcpy(&s3, inBlock + 3 * sizeof(s0), sizeof(s3));

    s0 = byteReverseWord32(s0);
    s1 = byteReverseWord32(s1);
    s2 = byteReverseWord32(s2);
    s3 = byteReverseWord32(s3);

    s0 ^= rk[0];
    s1 ^= rk[1];
    s2 ^= rk[2];
    s3 ^= rk[3];
   
    /*
     * Nr - 1 full rounds:
     */

    for (;;) {
        t0 =
            Te[GETBYTE(s0, 3)]  ^
            ROR(Te[GETBYTE(s1, 2)], 1)  ^
            ROR(Te[GETBYTE(s2, 1)], 2)  ^
            ROL(Te[GETBYTE(s3, 0)], 1)  ^
            rk[4];
        t1 =
            Te[GETBYTE(s1, 3)]  ^
            ROR(Te[GETBYTE(s2, 2)], 1)  ^
            ROR(Te[GETBYTE(s3, 1)], 2)  ^
            ROL(Te[GETBYTE(s0, 0)], 1)  ^
            rk[5];
        t2 =
            Te[GETBYTE(s2, 3)] ^
            ROR(Te[GETBYTE(s3, 2)], 1)  ^
            ROR(Te[GETBYTE(s0, 1)], 2)  ^
            ROL(Te[GETBYTE(s1, 0)], 1)  ^
            rk[6];
        t3 =
            Te[GETBYTE(s3, 3)] ^
            ROR(Te[GETBYTE(s0, 2)], 1)  ^
            ROR(Te[GETBYTE(s1, 1)], 2)  ^
            ROL(Te[GETBYTE(s2, 0)], 1)  ^
            rk[7];

        rk += 8;
        if (--r == 0) {
            break;
        }
        
        s0 =
            Te[GETBYTE(t0, 3)] ^
            ROR(Te[GETBYTE(t1, 2)], 1) ^
            ROR(Te[GETBYTE(t2, 1)], 2) ^
            ROL(Te[GETBYTE(t3, 0)], 1) ^
            rk[0];
        s1 =
            Te[GETBYTE(t1, 3)] ^
            ROR(Te[GETBYTE(t2, 2)], 1) ^
            ROR(Te[GETBYTE(t3, 1)], 2) ^
            ROL(Te[GETBYTE(t0, 0)], 1) ^
            rk[1];
        s2 =
            Te[GETBYTE(t2, 3)] ^
            ROR(Te[GETBYTE(t3, 2)], 1) ^
            ROR(Te[GETBYTE(t0, 1)], 2) ^
            ROL(Te[GETBYTE(t1, 0)], 1) ^
            rk[2];
        s3 =
            Te[GETBYTE(t3, 3)] ^
            ROR(Te[GETBYTE(t0, 2)], 1) ^
            ROR(Te[GETBYTE(t1, 1)], 2) ^
            ROL(Te[GETBYTE(t2, 0)], 1) ^
            rk[3];
    }

    /*
     * apply last round and
     * map cipher state to byte array block:
     */

    s0 =
        (Te[GETBYTE(t0, 3)] & 0x00ff0000) << 8 ^
        (Te[GETBYTE(t1, 2)] & 0x00ff0000) ^
        (Te[GETBYTE(t2, 1)] & 0x0000ff00) ^
        (Te[GETBYTE(t3, 0)] & 0x0000ff00) >> 8 ^
        rk[0];
    s1 =
        (Te[GETBYTE(t1, 3)] & 0x00ff0000) << 8 ^
        (Te[GETBYTE(t2, 2)] & 0x00ff0000) ^
        (Te[GETBYTE(t3, 1)] & 0x0000ff00) ^
        (Te[GETBYTE(t0, 0)] & 0x0000ff00) >> 8 ^
        rk[1];
    s2 =
        (Te[GETBYTE(t2, 3)] & 0x00ff0000) << 8 ^
        (Te[GETBYTE(t3, 2)] & 0x00ff0000) ^
        (Te[GETBYTE(t0, 1)] & 0x0000ff00) ^
        (Te[GETBYTE(t1, 0)] & 0x0000ff00) >> 8 ^
        rk[2];
    s3 =
        (Te[GETBYTE(t3, 3)] & 0x00ff0000) << 8 ^
        (Te[GETBYTE(t0, 2)] & 0x00ff0000) ^
        (Te[GETBYTE(t1, 1)] & 0x0000ff00) ^
        (Te[GETBYTE(t2, 0)] & 0x0000ff00) >> 8 ^
        rk[3];

    s0 = byteReverseWord32(s0);
    s1 = byteReverseWord32(s1);
    s2 = byteReverseWord32(s2);
    s3 = byteReverseWord32(s3);

    memcpy(outBlock,                  &s0, sizeof(s0));
    memcpy(outBlock + sizeof(s0),     &s1, sizeof(s1));
    memcpy(outBlock + 2 * sizeof(s0), &s2, sizeof(s2));
    memcpy(outBlock + 3 * sizeof(s0), &s3, sizeof(s3));
}

void aesDecrypt(AES* aes, const unsigned char *inBlock, unsigned char *outBlock) {
    unsigned int s0, s1, s2, s3;
    unsigned int t0, t1, t2, t3;
    unsigned int r = aes->rounds >> 1;

    const unsigned int *rk = aes->key;
    /*
     * map byte array block to cipher state
     * and add initial round key:
     */
    memcpy(&s0, inBlock,                  sizeof(s0));
    memcpy(&s1, inBlock + sizeof(s0),     sizeof(s1));
    memcpy(&s2, inBlock + 2 * sizeof(s0), sizeof(s2));
    memcpy(&s3, inBlock + 3 * sizeof(s0), sizeof(s3));

    s0 = byteReverseWord32(s0);
    s1 = byteReverseWord32(s1);
    s2 = byteReverseWord32(s2);
    s3 = byteReverseWord32(s3);    

    s0 ^= rk[0];
    s1 ^= rk[1];
    s2 ^= rk[2];
    s3 ^= rk[3];
   
    /*
     * Nr - 1 full rounds:
     */

    for (;;) {
        t0 =
            Td[0][GETBYTE(s0, 3)] ^
            ROR(Td[0][GETBYTE(s3, 2)], 1) ^
            ROR(Td[0][GETBYTE(s2, 1)], 2) ^
            ROL(Td[0][GETBYTE(s1, 0)], 1) ^
            rk[4];
        t1 =
            Td[0][GETBYTE(s1, 3)] ^
            ROR(Td[0][GETBYTE(s0, 2)], 1) ^
            ROR(Td[0][GETBYTE(s3, 1)], 2) ^
            ROL(Td[0][GETBYTE(s2, 0)], 1) ^
            rk[5];
        t2 =
            Td[0][GETBYTE(s2, 3)] ^
            ROR(Td[0][GETBYTE(s1, 2)], 1) ^
            ROR(Td[0][GETBYTE(s0, 1)], 2) ^
            ROL(Td[0][GETBYTE(s3, 0)], 1) ^
            rk[6];
        t3 =
            Td[0][GETBYTE(s3, 3)] ^
            ROR(Td[0][GETBYTE(s2, 2)], 1) ^
            ROR(Td[0][GETBYTE(s1, 1)], 2) ^
            ROL(Td[0][GETBYTE(s0, 0)], 1) ^
            rk[7];

        rk += 8;
        if (--r == 0) {
            break;
        }

        s0 =
            Td[0][GETBYTE(t0, 3)] ^
            ROR(Td[0][GETBYTE(t3, 2)], 1) ^
            ROR(Td[0][GETBYTE(t2, 1)], 2) ^
            ROL(Td[0][GETBYTE(t1, 0)], 1) ^
            rk[0];
        s1 =
            Td[0][GETBYTE(t1, 3)] ^
            ROR(Td[0][GETBYTE(t0, 2)], 1) ^
            ROR(Td[0][GETBYTE(t3, 1)], 2) ^
            ROL(Td[0][GETBYTE(t2, 0)], 1) ^
            rk[1];
        s2 =
            Td[0][GETBYTE(t2, 3)] ^
            ROR(Td[0][GETBYTE(t1, 2)], 1) ^
            ROR(Td[0][GETBYTE(t0, 1)], 2) ^
            ROL(Td[0][GETBYTE(t3, 0)], 1) ^
            rk[2];
        s3 =
            Td[0][GETBYTE(t3, 3)] ^
            ROR(Td[0][GETBYTE(t2, 2)], 1) ^
            ROR(Td[0][GETBYTE(t1, 1)], 2) ^
            ROL(Td[0][GETBYTE(t0, 0)], 1) ^
            rk[3];
    }
    /*
     * apply last round and
     * map cipher state to byte array block:
     */
    s0 =
        (Td[1][GETBYTE(t0, 3)] & 0xff000000) ^
        (Td[1][GETBYTE(t3, 2)] & 0x00ff0000) ^
        (Td[1][GETBYTE(t2, 1)] & 0x0000ff00) ^
        (Td[1][GETBYTE(t1, 0)] & 0x000000ff) ^
        rk[0];
    s1 =
        (Td[1][GETBYTE(t1, 3)] & 0xff000000) ^
        (Td[1][GETBYTE(t0, 2)] & 0x00ff0000) ^
        (Td[1][GETBYTE(t3, 1)] & 0x0000ff00) ^
        (Td[1][GETBYTE(t2, 0)] & 0x000000ff) ^
        rk[1];
    s2 =
        (Td[1][GETBYTE(t2, 3)] & 0xff000000) ^
        (Td[1][GETBYTE(t1, 2)] & 0x00ff0000) ^
        (Td[1][GETBYTE(t0, 1)] & 0x0000ff00) ^
        (Td[1][GETBYTE(t3, 0)] & 0x000000ff) ^
        rk[2];
    s3 =
        (Td[1][GETBYTE(t3, 3)] & 0xff000000) ^
        (Td[1][GETBYTE(t2, 2)] & 0x00ff0000) ^
        (Td[1][GETBYTE(t1, 1)] & 0x0000ff00) ^
        (Td[1][GETBYTE(t0, 0)] & 0x000000ff) ^
        rk[3];

    s0 = byteReverseWord32(s0);
    s1 = byteReverseWord32(s1);
    s2 = byteReverseWord32(s2);
    s3 = byteReverseWord32(s3);

    memcpy(outBlock,                  &s0, sizeof(s0));
    memcpy(outBlock + sizeof(s0),     &s1, sizeof(s1));
    memcpy(outBlock + 2 * sizeof(s0), &s2, sizeof(s2));
    memcpy(outBlock + 3 * sizeof(s0), &s3, sizeof(s3));
}

void aesEcbEncrypt(AES* aes, unsigned char *output, const unsigned char *input, unsigned int inputSize) {
    unsigned int blocks = inputSize / SMALL_AES_BLOCK_SIZE;

    while (blocks--) {        
        aesEncrypt(aes, input, (unsigned char *)aes->reg);
        memcpy(output, aes->reg, SMALL_AES_BLOCK_SIZE);

        output += SMALL_AES_BLOCK_SIZE;
        input  += SMALL_AES_BLOCK_SIZE; 
    }
}

void aesEcbDecrypt(AES* aes, unsigned char *output, const unsigned char *input, unsigned int inputSize) {
    unsigned int blocks = inputSize / SMALL_AES_BLOCK_SIZE;

    while (blocks--) {        
        aesDecrypt(aes, input, (unsigned char *)aes->reg);        
        memcpy(output, aes->reg, SMALL_AES_BLOCK_SIZE);

        output += SMALL_AES_BLOCK_SIZE;
        input  += SMALL_AES_BLOCK_SIZE; 
    }
}

unsigned int byteReverseWord32(unsigned int value) {
    return (rotrFixed(value, 8U) & 0xff00ff00) | (rotlFixed(value, 8U) & 0x00ff00ff);
}

void byteReverseWords(unsigned int *out, const unsigned int *in, unsigned int byteCount) {
    unsigned int count = byteCount/sizeof(unsigned int), i;    
    
    for (i = 0; i < count; i++)
        out[i] = byteReverseWord32(in[i]);
}

unsigned int rotlFixed(unsigned int x, unsigned int y) {
    return (x << y) | (x >> (sizeof(y) * 8 - y));
}   


unsigned int rotrFixed(unsigned int x, unsigned int y) {
    return (x >> y) | (x << (sizeof(y) * 8 - y));
}