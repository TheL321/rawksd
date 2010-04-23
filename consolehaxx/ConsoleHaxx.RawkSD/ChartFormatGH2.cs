﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

namespace ConsoleHaxx.RawkSD
{
	public class ChartFormatGH2 : IChartFormat
	{
		public const string ChartFile = "chart";

		public static readonly ChartFormatGH2 Instance;
		static ChartFormatGH2()
		{
			Instance = new ChartFormatGH2();
		}

		public override int ID {
			get { return 0x02; }
		}

		public override string Name {
			get { return "Guitar Hero 2 / 80s"; }
		}

		public void Create(FormatData data, Stream chart)
		{
			data.SetStream(this, ChartFile, chart);
		}

		public override bool Writable {
			get { return false; }
		}

		public override bool Readable {
			get { return true; }
		}

		public override ChartFormat DecodeChart(FormatData data)
		{
			if (!data.HasStream(this, ChartFile))
				throw new FormatException();

			Stream stream = data.GetStream(this, ChartFile);
			ChartFormat chart = ChartFormat.Create(stream);
			data.CloseStream(stream);
			return chart;
		}

		public override void EncodeChart(ChartFormat data, FormatData destination, ProgressIndicator progress)
		{
			throw new NotImplementedException();
		}
	}
}
