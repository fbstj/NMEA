using System;
using System.Collections.Generic;

namespace fbstj.Serial
{
	public delegate void Consumer<T>(T value);
	public delegate S Converter<S,T>(T value);

	public static class NMEA
	{
		// the three characters used to make packets
		public const char DOLLAR = '$', DELIMIT = ',', STAR = '*';

		// the string formatting for the checksum
		private const string CRC_FORMAT = "X2";

		// The method that converts the contents of an NMEA packet into a single byte checksum
		public static Converter<byte, string> CRC = delegate(string value)
		{
			byte init = 0xFF;
			foreach (byte b in value)
				init ^= b;
			return init;
		};

		/// <summary>Parse a string into a packet structure</summary>
		public static Packet Parse(string packet)
		{
			if (packet[0] != DOLLAR)
				throw new Exception("String is not a pakcet");

			string[] args = packet.Split(DELIMIT, STAR);

			Packet p = new Packet(args[0].Substring(1));

			for (int i = 1; i < args.Length - 1; i++)
				p.Add(args[i]);

			byte crc = Convert.ToByte(args[args.Length - 1], 16);

			if (crc != p.CRC)
				throw new Exception("Packet has incorrect checksum");

			return p;
		}

		/// <summary>Packet factory that adds multiple arguments to the pakcet</summary>
		public static Packet Raw(string command, params string[] args)
		{
			Packet p = new Packet(command);
			foreach (string arg in args)
				p.Add(arg);
			return Parse(p.ToString());
		}

		/// <summary>Consume a serially received Packet</summary>
		public static event Consumer<Packet> Consume;

		private static byte _rec_state;
		private static string _rec_buf;
		/// <summary>NMEA serial state machine</summary>
		public static void Receive_Char(byte b)
		{
			char ch = (char)b;
			if (ch == '$')
			{	// the start of a NMEA
				_rec_buf = "";
				_rec_state = 0;
			}
			else if (ch == '*')
			{	// two more characters for the CRC
				_rec_state = 3;
			}
			_rec_buf += ch;
			_rec_state--;
			if (_rec_state == 0)
				_recieve(_rec_buf);
		}

		private static Packet _latest;
		private static bool _received;
		/// <summary>When received, try parse and consume</summary>
		private static void _recieve(string str)
		{
			_received = false;
			try
			{
				_latest = Parse(_rec_buf);
				_received = true;
			}
			catch
			{
				_received = false;
				return;
			}
			if (Consume != null)
				Consume(_latest);
		}

		/// <summary>Block until a packet is received</summary>
		public static Packet Poll()
		{
			_received = false;
			while (!_received) ;
			return _latest;
		}

		public struct Packet
		{
			private List<string> _arg;

			public Packet(string command)
			{
				_arg = new List<string>();
				_arg.Add(command);
			}

			public string this[int i]
			{
				get { if (_arg.Count > i) return _arg[i]; return ""; }
				set { if (_arg.Count < i) Add(value); _arg[i] = value; ; }
			}

			/// <summary>The command string</summary>
			public string Command { get { return this[0]; } }

			/// <summary>The number of arguments added</summary>
			public int Count { get { return _arg.Count - 1; } }

			/// <summary>Checksum</summary>
			public byte CRC { get { return NMEA.CRC(_str()); } }

			/// <summary>Add an argument to the string</summary>
			public void Add(string arg) { _arg.Add(arg); }

			/// <summary>Add a formatted argument to the string</summary>
			public void Add(long arg, string fmt) { Add(arg.ToString(fmt)); }

			/// <summary>Add a boolean argument to the string</summary>
			public void Add(bool arg, string onTrue, string onFalse) { Add(arg ? onTrue : onFalse); }

			/// <summary>Retreive an integer argument</summary>
			public long Argument(int id, int radix) { return Convert.ToInt64(this[id], radix); }

			/// <summary>Retrieve a boolean argument</summary>
			public bool Argument(int id, string onTrue) { return this[id] == onTrue; }

			private string _str() { return String.Join("" + DELIMIT, _arg.ToArray()); }

			public override string ToString() { return DOLLAR + _str() + STAR + CRC.ToString(CRC_FORMAT); }
		}
	}
}
