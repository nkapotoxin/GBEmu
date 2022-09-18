#include <sound.h>
#include <gbio.h>
#include <ram.h>
#include <SDL2/SDL.h>

static sound_context ctx;
int sample_rate = 44100;

#define RATE (ctx.snd.rate)
#define WAVE (ctx.snd.wave) /* ctx.snd_mem[0x30] */
#define S1 (ctx.snd.ch[0])
#define S2 (ctx.snd.ch[1])
#define S3 (ctx.snd.ch[2])
#define S4 (ctx.snd.ch[3])

int sound_init() {
	ctx.threaded = 0;
    SDL_AudioSpec as = {0}, ob;
	SDL_InitSubSystem(SDL_INIT_AUDIO);

    as.freq = sample_rate;
	as.format = AUDIO_U8;
	as.channels = 2;
	as.samples = sample_rate / 60;
	as.userdata = 0;
	int i;
	for (i = 1; i < as.samples; i<<=1);
	as.samples = i;
	as.callback = ctx.threaded ? sound_fill : NULL;
	ctx.device = SDL_OpenAudioDevice(NULL, 0, &as,
		&ob, SDL_AUDIO_ALLOW_CHANNELS_CHANGE|SDL_AUDIO_ALLOW_FREQUENCY_CHANGE);

    if (!ctx.device) {
		fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
		exit(-1);
	}

	ctx.hz = ob.freq;
	ctx.stereo = ob.channels - 1;
	ctx.len = ob.size;
	ctx.buf = malloc(ctx.len);
	ctx.pos = 0;
	ctx.tick = 0;
	memset(ctx.buf, 0, ctx.len);
	SDL_PauseAudioDevice(ctx.device, 0);

	sound_reset();
}

void sound_tick(int cpu_cycles) {
	ctx.tick += cpu_cycles;
}

void sound_fill(void *userdata, unsigned char *stream, int len) {
    memcpy(stream, ctx.buf, len);
	ctx.sound_done = 1;
}

int sound_submit()
{
	int res,min;
	if (!ctx.buf || ctx.paused) {
		ctx.pos = 0;
		return 0;
	}
	
	if(ctx.threaded) {
		if(ctx.pos < ctx.len) return 1;
		while(!ctx.sound_done) SDL_Delay(1);
		ctx.sound_done = 0;
		ctx.pos = 0;
		return 1;
	}

	min = ctx.len*2;
	res = SDL_QueueAudio(ctx.device, ctx.buf, ctx.pos) == 0;
	ctx.pos = 0;
	while (res && SDL_GetQueuedAudioSize(ctx.device) > min)
		SDL_Delay(1);
	return res;
}

void sound_mix() {
	int s, l, r, f, n;

	if (!RATE || ctx.tick < RATE) return;

	// printf("cpu.snd %d\n", ctx.tick);
	for (; ctx.tick >= RATE; ctx.tick -= RATE)
	{
		l = r = 0;

		if (S1.on)
		{
			s = sqwave[R_NR11>>6][(S1.pos>>18)&7] & S1.envol;
			S1.pos += S1.freq;
			if ((R_NR14 & 64) && ((S1.cnt += RATE) >= S1.len))
				S1.on = 0;
			if (S1.enlen && (S1.encnt += RATE) >= S1.enlen)
			{
				S1.encnt -= S1.enlen;
				S1.envol += S1.endir;
				if (S1.envol < 0) S1.envol = 0;
				if (S1.envol > 15) S1.envol = 15;
			}
			if (S1.swlen && (S1.swcnt += RATE) >= S1.swlen)
			{
				S1.swcnt -= S1.swlen;
				f = S1.swfreq;
				n = (R_NR10 & 7);
				if (R_NR10 & 8) f -= (f >> n);
				else f += (f >> n);
				if (f > 2047)
					S1.on = 0;
				else
				{
					S1.swfreq = f;
					R_NR13 = f;
					R_NR14 = (R_NR14 & 0xF8) | (f>>8);
					s1_freq_d(2048 - f);
				}
			}
			s <<= 2;
			if (R_NR51 & 0x01) r += s;
			if (R_NR51 & 0x10) l += s;
		}
		
		if (S2.on)
		{
			s = sqwave[R_NR21>>6][(S2.pos>>18)&7] & S2.envol;
			S2.pos += S2.freq;
			if ((R_NR24 & 64) && ((S2.cnt += RATE) >= S2.len))
				S2.on = 0;
			if (S2.enlen && (S2.encnt += RATE) >= S2.enlen)
			{
				S2.encnt -= S2.enlen;
				S2.envol += S2.endir;
				if (S2.envol < 0) S2.envol = 0;
				if (S2.envol > 15) S2.envol = 15;
			}
			s <<= 2;
			if (R_NR51 & 0x02) r += s;
			if (R_NR51 & 0x20) l += s;
		}
		
		if (S3.on)
		{
			s = WAVE[(S3.pos>>22) & 15];
			if (S3.pos & (1<<21)) s &= 15;
			else s >>= 4;
			s -= 8;
			S3.pos += S3.freq;
			if ((R_NR34 & 64) && ((S3.cnt += RATE) >= S3.len))
				S3.on = 0;
			if (R_NR32 & 96) s <<= (3 - ((R_NR32>>5)&3));
			else s = 0;
			if (R_NR51 & 0x04) r += s;
			if (R_NR51 & 0x40) l += s;
		}

		if (S4.on)
		{
			if (R_NR43 & 8) s = 1 & (noise7[
				(S4.pos>>20)&15] >> (7-((S4.pos>>17)&7)));
			else s = 1 & (noise15[
				(S4.pos>>20)&4095] >> (7-((S4.pos>>17)&7)));
			s = (-s) & S4.envol;
			S4.pos += S4.freq;
			if ((R_NR44 & 64) && ((S4.cnt += RATE) >= S4.len))
				S4.on = 0;
			if (S4.enlen && (S4.encnt += RATE) >= S4.enlen)
			{
				S4.encnt -= S4.enlen;
				S4.envol += S4.endir;
				if (S4.envol < 0) S4.envol = 0;
				if (S4.envol > 15) S4.envol = 15;
			}
			s += s << 1;
			if (R_NR51 & 0x08) r += s;
			if (R_NR51 & 0x80) l += s;
		}
		
		l *= (R_NR50 & 0x07);
		r *= ((R_NR50 & 0x70)>>4);
		l >>= 4;
		r >>= 4;
		
		if (l > 127) l = 127;
		else if (l < -128) l = -128;
		if (r > 127) r = 127;
		else if (r < -128) r = -128;

		if (ctx.buf)
		{
			if (ctx.pos >= ctx.len)
				sound_submit();
			if (ctx.stereo)
			{
				ctx.buf[ctx.pos++] = l+128;
				ctx.buf[ctx.pos++] = r+128;
			}
			else ctx.buf[ctx.pos++] = ((l+r)>>1)+128;
		}
	}
	R_NR52 = (R_NR52&0xF0) | S1.on | (S2.on<<1) | (S3.on<<2) | (S4.on<<3);
}

u8 sound_read(u16 address) {
	sound_mix();
    return ctx.snd_mem[address-0xFF00];
}

void s1_init()
{
	S1.swcnt = 0;
	S1.swfreq = ((R_NR14&7)<<8) + R_NR13;
	S1.envol = R_NR12 >> 4;
	S1.endir = (R_NR12>>3) & 1;
	S1.endir |= S1.endir - 1;
	S1.enlen = (R_NR12 & 7) << 15;
	if (!S1.on) S1.pos = 0;
	S1.on = 1;
	S1.cnt = 0;
	S1.encnt = 0;
}

void s2_init()
{
	S2.envol = R_NR22 >> 4;
	S2.endir = (R_NR22>>3) & 1;
	S2.endir |= S2.endir - 1;
	S2.enlen = (R_NR22 & 7) << 15;
	if (!S2.on) S2.pos = 0;
	S2.on = 1;
	S2.cnt = 0;
	S2.encnt = 0;
}

void s3_init()
{
	int i;
	if (!S3.on) S3.pos = 0;
	S3.cnt = 0;
	S3.on = R_NR30 >> 7;
	if (S3.on) for (i = 0; i < 16; i++)
		ctx.snd_mem[i+0x30] = 0x13 ^ ctx.snd_mem[i+0x31];
}

void s4_init()
{
	S4.envol = R_NR42 >> 4;
	S4.endir = (R_NR42>>3) & 1;
	S4.endir |= S4.endir - 1;
	S4.enlen = (R_NR42 & 7) << 15;
	S4.on = 1;
	S4.pos = 0;
	S4.cnt = 0;
	S4.encnt = 0;
}

void sound_write(u16 address, u8 b) {
	if (!(R_NR52 & 128) && (address - 0xFF00) != RI_NR52) return;
	if (((address - 0xFF00) & 0xF0) == 0x30)
	{
		if (S3.on) sound_mix();
		if (!S3.on)
			WAVE[address - 0xFF00 -0x30] = ctx.snd_mem[address- 0xFF00] = b;
		return;
	}
	sound_mix();
	switch (address-0xFF00)
	{
	case RI_NR10:
		R_NR10 = b;
		S1.swlen = ((R_NR10>>4) & 7) << 14;
		S1.swfreq = ((R_NR14&7)<<8) + R_NR13;
		break;
	case RI_NR11:
		R_NR11 = b;
		S1.len = (64-(R_NR11&63)) << 13;
		break;
	case RI_NR12:
		R_NR12 = b;
		S1.envol = R_NR12 >> 4;
		S1.endir = (R_NR12>>3) & 1;
		S1.endir |= S1.endir - 1;
		S1.enlen = (R_NR12 & 7) << 15;
		break;
	case RI_NR13:
		R_NR13 = b;
		s1_freq();
		break;
	case RI_NR14:
		R_NR14 = b;
		s1_freq();
		if (b & 128) s1_init();
		break;
	case RI_NR21:
		R_NR21 = b;
		S2.len = (64-(R_NR21&63)) << 13;
		break;
	case RI_NR22:
		R_NR22 = b;
		S2.envol = R_NR22 >> 4;
		S2.endir = (R_NR22>>3) & 1;
		S2.endir |= S2.endir - 1;
		S2.enlen = (R_NR22 & 7) << 15;
		break;
	case RI_NR23:
		R_NR23 = b;
		s2_freq();
		break;
	case RI_NR24:
		R_NR24 = b;
		s2_freq();
		if (b & 128) s2_init();
		break;
	case RI_NR30:
		R_NR30 = b;
		if (!(b & 128)) S3.on = 0;
		break;
	case RI_NR31:
		R_NR31 = b;
		S3.len = (256-R_NR31) << 13;
		break;
	case RI_NR32:
		R_NR32 = b;
		break;
	case RI_NR33:
		R_NR33 = b;
		s3_freq();
		break;
	case RI_NR34:
		R_NR34 = b;
		s3_freq();
		if (b & 128) s3_init();
		break;
	case RI_NR41:
		R_NR41 = b;
		S4.len = (64-(R_NR41&63)) << 13;
		break;
	case RI_NR42:
		R_NR42 = b;
		S4.envol = R_NR42 >> 4;
		S4.endir = (R_NR42>>3) & 1;
		S4.endir |= S4.endir - 1;
		S4.enlen = (R_NR42 & 7) << 15;
		break;
	case RI_NR43:
		R_NR43 = b;
		s4_freq();
		break;
	case RI_NR44:
		R_NR44 = b;
		if (b & 128) s4_init();
		break;
	case RI_NR50:
		R_NR50 = b;
		break;
	case RI_NR51:
		R_NR51 = b;
		break;
	case RI_NR52:
		R_NR52 = b;
		if (!(R_NR52 & 128))
			sound_off();
		break;
	default:
		return;
	}
}

void sound_cleanup() {
	SDL_CloseAudio();
}

void sound_close() {
	SDL_CloseAudioDevice(ctx.device);
}

void sound_pause(int dopause) {
	ctx.paused = dopause;
	SDL_PauseAudioDevice(ctx.device, ctx.paused);
}

void sound_reset() {
	memset(&ctx.snd, 0, sizeof ctx.snd);
	if (ctx.hz) {
		ctx.snd.rate = (1<<21) / ctx.hz;
	} else {
		ctx.snd.rate = 0;
	}

	printf("rate %d\n", ctx.snd.rate);

	memcpy(ctx.snd.wave, dmgwave, 16);
	memcpy(&ctx.snd_mem[0x30], ctx.snd.wave, 16);
	sound_off();
	R_NR52 = 0xF1;
}

void s1_freq_d(int d)
{
	if (ctx.snd.rate > (d<<4)) ctx.snd.ch[0].freq = 0;
	else ctx.snd.ch[1].freq = (ctx.snd.rate << 17)/d;
}

void s1_freq()
{
	s1_freq_d(2048 - (((R_NR14&7)<<8) + R_NR13));
}

void s2_freq()
{
	int d = 2048 - (((R_NR24&7)<<8) + R_NR23);
	if (ctx.snd.rate > (d<<4)) ctx.snd.ch[1].freq = 0;
	else ctx.snd.ch[1].freq = (ctx.snd.rate << 17)/d;
}

void s3_freq()
{
	int d = 2048 - (((R_NR34&7)<<8) + R_NR33);
	if (ctx.snd.rate > (d<<3)) ctx.snd.ch[2].freq = 0;
	else ctx.snd.ch[2].freq = (ctx.snd.rate << 21)/d;
}

void s4_freq()
{
	ctx.snd.ch[3].freq = (freqtab[R_NR43&7] >> (R_NR43 >> 4)) * ctx.snd.rate;
	if (ctx.snd.ch[3].freq >> 18) ctx.snd.ch[3].freq = 1<<18;
}

void sound_dirty()
{
	S1.swlen = ((R_NR10>>4) & 7) << 14;
	S1.len = (64-(R_NR11&63)) << 13;
	S1.envol = R_NR12 >> 4;
	S1.endir = (R_NR12>>3) & 1;
	S1.endir |= S1.endir - 1;
	S1.enlen = (R_NR12 & 7) << 15;
	s1_freq();
	S2.len = (64-(R_NR21&63)) << 13;
	S2.envol = R_NR22 >> 4;
	S2.endir = (R_NR22>>3) & 1;
	S2.endir |= S2.endir - 1;
	S2.enlen = (R_NR22 & 7) << 15;
	s2_freq();
	S3.len = (256-R_NR31) << 20;
	s3_freq();
	S4.len = (64-(R_NR41&63)) << 13;
	S4.envol = R_NR42 >> 4;
	S4.endir = (R_NR42>>3) & 1;
	S4.endir |= S4.endir - 1;
	S4.enlen = (R_NR42 & 7) << 15;
	s4_freq();
}

void sound_off() {
	memset(&ctx.snd.ch[0], 0, sizeof ctx.snd.ch[0]);
	memset(&ctx.snd.ch[1], 0, sizeof ctx.snd.ch[1]);
	memset(&ctx.snd.ch[2], 0, sizeof ctx.snd.ch[2]);
	memset(&ctx.snd.ch[3], 0, sizeof ctx.snd.ch[3]);
	R_NR10 = 0x80;
	R_NR11 = 0xBF;
	R_NR12 = 0xF3;
	R_NR14 = 0xBF;
	R_NR21 = 0x3F;
	R_NR22 = 0x00;
	R_NR24 = 0xBF;
	R_NR30 = 0x7F;
	R_NR31 = 0xFF;
	R_NR32 = 0x9F;
	R_NR34 = 0xBF;
	R_NR41 = 0xFF;
	R_NR42 = 0x00;
	R_NR43 = 0x00;
	R_NR44 = 0xBF;
	R_NR50 = 0x77;
	R_NR51 = 0xF3;
	R_NR52 = 0x70;

	sound_dirty();
}

