using System;
using System.IO;
using System.Runtime.InteropServices;

namespace XLanceDLL
{
	/// <summary>
	/// Summary description for BinFile.
	/// </summary>
	public class StructFile
	{
		private object				_oStruct	= null;
		private System.Type	_oType		= null;
		private string				_File			= null;
		private FileStream		_fs			= null;

		public StructFile(string szFile, System.Type type)
		{
			_File = szFile;
			_oType = type;
		}

		private void LoadFileStream (FileMode FileMode, FileAccess FileAccess, FileShare FileShare)
		{
			if (_fs == null)
			{
				try
				{
					_fs = new FileStream(_File, FileMode, FileAccess, FileShare);
				}
				catch (Exception ex)
				{
					throw ex;
				}
			}
		}

		public bool EOF				//End of File
		{
			get
			{
				if (_fs != null) 
				{
					if (_fs.Position >= _fs.Length) 
						Close();					
				}

				return _fs == null;
			}
		}

		private byte[] StructToByteArray()
		{
			try
			{
			// This function copys the structure data into a byte[]
			byte[] buffer = new byte[Marshal.SizeOf(_oStruct)];									//Set the buffer ot the correct size
			
			GCHandle h = GCHandle.Alloc(buffer , GCHandleType.Pinned);					//Allocate the buffer to memory and pin it so that GC cannot use the space (Disable GC)
			Marshal.StructureToPtr(_oStruct, h.AddrOfPinnedObject(), false);				// copy the struct into int byte[] mem alloc 
			h.Free();																								//Allow GC to do its job
	
			return buffer;																							// return the byte[] . After all thats why we are here right.
			}
			catch (Exception ex)
			{
				throw ex;
			}
		}


		public bool WriteStructure(object oStruct)
		{
			_oStruct = oStruct;
			try
			{
				byte[] buf = StructToByteArray();

				BinaryWriter bw = new BinaryWriter(_fs);

				bw.Write(buf);

				bw.Close();
				bw = null;

				return true;
			}
			catch (Exception ex)
			{
				throw ex;
			}
		}

		public object GetNextStructureValue()
		{
			byte[] buffer = new byte [Marshal.SizeOf(_oType)];
	
			object oReturn = null;

			try
			{
				if (EOF) return null;

				_fs.Read(buffer, 0, buffer.Length);

				GCHandle handle = GCHandle.Alloc(buffer, GCHandleType.Pinned);
				oReturn = (object)Marshal.PtrToStructure(handle.AddrOfPinnedObject(), _oType);
				handle.Free();

				if (_fs.Position >= _fs.Length) 
					Close();

				return oReturn;
			}
			catch (Exception ex)
			{
				throw ex;
			}
		}

		public void Close()			//Close the file stream
		{
			if (_fs != null)
			{
				_fs.Close();
				_fs = null;
			}
			GC.Collect();
		}

		public void Open(FileMode FileMode, FileAccess FileAccess, FileShare FileShare)
		{
			LoadFileStream(FileMode, FileAccess, FileShare);
		}
	}
}
