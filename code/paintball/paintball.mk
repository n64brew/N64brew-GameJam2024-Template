
ASSETS_LIST += \
	filesystem/paintball/char.t3dm \
	filesystem/paintball/bullet.t3dm \
	filesystem/paintball/splash.ia4.sprite \
	filesystem/paintball/arrow.ia4.sprite \
	filesystem/paintball/marker.ia4.sprite \
	filesystem/paintball/shadow.t3dm \
	filesystem/paintball/shadow.i8.sprite \
	filesystem/paintball/FingerPaint-Regular.font64 \
	filesystem/paintball/FingerPaint-Regular-Medium.font64 \
	filesystem/paintball/FingerPaint-Regular-Big.font64 \
	filesystem/paintball/fire.wav64 \
	filesystem/paintball/capture.wav64 \
	filesystem/paintball/hit.wav64

filesystem/paintball/FingerPaint-Regular.font64: MKFONT_FLAGS += --outline 1 --size 12
filesystem/paintball/FingerPaint-Regular-Medium.font64: MKFONT_FLAGS += --outline 2 --size 24
filesystem/paintball/FingerPaint-Regular-Big.font64: MKFONT_FLAGS += --outline 2 --size 36