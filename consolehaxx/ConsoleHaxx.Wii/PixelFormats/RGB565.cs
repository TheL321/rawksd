﻿#region Using Shortcuts
using s8 = System.SByte;
using u8 = System.Byte;
using s16 = System.Int16;
using u16 = System.UInt16;
using s32 = System.Int32;
using u32 = System.UInt32;
using s64 = System.Int64;
using u64 = System.UInt64;
#endregion

using System;
using System.Drawing;
using System.IO;
using ConsoleHaxx.Common;

namespace ConsoleHaxx.Wii
{
	public class RGB565 : PixelEncoding
	{
		public override uint ID
		{
			get { return 0x04; }
		}

		public override uint BitsPerPixel
		{
			get { return 0x10; }
		}

		public override Size Tilesize
		{
			get { return new Size(4, 4); }
		}

		public override Bitmap DecodeImage(Stream stream, uint width, uint height)
		{
			Bitmap image = new Bitmap((int)width, (int)height);

			EndianReader reader = new EndianReader(stream, Endianness.BigEndian);

			for (int y = 0; y < height; y += 4) {
				for (int x = 0; x < width; x += 4) {
					for (int y2 = 0; y2 < 4; y2++) {
						for (int x2 = 0; x2 < 4; x2++) {
							int pixelx = x + x2;
							int pixely = y + y2;
							if (pixelx < width && pixely < height) // Clip/ignore bytes that go beyond the size of the image
								image.SetPixel(pixelx, pixely, GetPixel(reader.ReadUInt16()));
						}
					}
				}
			}

			return image;
		}

		public static Color GetPixel(ushort pixel)
		{
			return Color.FromArgb(255, (((pixel) & 0x1F) << 3) & 0xff, (((pixel >> 5) & 0x3F) << 2) & 0xff, (((pixel >> 11) & 0x1F) << 3) & 0xff);
		}

		public static ushort GetPixel(Color colour)
		{
			ushort pixel = 0;
			pixel |= (ushort)((colour.R >> 3) & 0x001F);
			pixel |= (ushort)((colour.G << 3) & 0x07E0);
			pixel |= (ushort)((colour.B << 8) & 0xF800);
			return pixel;
		}

		public override void EncodeImage(Stream stream, Bitmap image)
		{
			EndianReader writer = new EndianReader(stream, Endianness.BigEndian);

			for (int y = 0; y < image.Height; y += 4) {
				for (int x = 0; x < image.Width; x += 4) {
					for (int y2 = 0; y2 < 4; y2++) {
						for (int x2 = 0; x2 < 4; x2++) {
							int pixelx = x + x2;
							int pixely = y + y2;
							if (pixelx < image.Width && pixely < image.Height) {
								writer.Write(GetPixel(image.GetPixel(pixelx, pixely)));
							}
						}
					}
				}
			}
		}
	}
}
