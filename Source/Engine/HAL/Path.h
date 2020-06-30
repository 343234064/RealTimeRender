#pragma once

#include "Global/GlobalType.h"
#include "Global/Utilities/String.h"


struct Path
{
	// e.g. C:/dir1/dir2/dir3/../../file -> C:/dir1/file
	static bool CollapseRelativePath(String& Path)
	{
		const TChar ParentDir[] = TEXTS("/..");
		const int32 ParentDirLen = 3;

		//Search '/..'
		while (true)
		{
			if (Path.Len() == 0) break;
			if (Path.StartWith(TEXTS(".."), false) || Path.StartWith(TEXTS("/.."), false)) return false;

			int32 ParentSlashIndex = Path.Find(ParentDir, 0, false);
			int32 ParentIndex = ParentSlashIndex;
			if (ParentSlashIndex == -1) break;

			//Search '/'
			while (true)
			{
				ParentSlashIndex = std::max(0, Path.FindFromEnd(TEXTS("/"), ParentSlashIndex - 1, false));

				if (ParentSlashIndex == 0) break;

				if ((ParentIndex - ParentSlashIndex) > 1 && (Path[ParentSlashIndex + 1] != TEXTS('.') || Path[ParentSlashIndex + 1] != TEXTS('/')))
					break;
			}

			int32 ColonInedx = Path.Find(TEXTS(":"), ParentSlashIndex, false);
			if (ColonInedx >= 0 && ColonInedx < ParentIndex) return false;

			//Remove the string from '/' to '/..'
			Path.RemoveAt(ParentSlashIndex, ParentIndex - ParentSlashIndex + ParentDirLen, false);
		}

		Path.Replace(TEXTS("./"), TEXTS(""));
		Path[Path.Len()] = 0;

		return true;
	}

	static bool IsRelativePath(const String& Path)
	{
		const uint32 PathLen = Path.Len();
		bool IsRootPath = PathLen &&
			(PathLen > 2 && ((Path[1] == ':' && Chars::IsAlpha(Path[0]))));

		return !IsRootPath;
	}

	static String NormalizeFilePath(const TChar* FilePath)
	{
		String Path(FilePath);
		Path.Replace(TEXTS("\\"), TEXTS("/"));
		//Path.Replace(TEXTS("//"), TEXTS("/"));

		if (IsRelativePath(Path))
		{
			String BasePath(Path::BaseDir());
			BasePath.AppendPath(*Path, Path.Len());
			Path = std::move(BasePath);
		}

		CollapseRelativePath(Path);

		if (Path.Len() == 0)
		{
			Path = TEXTS("/");
		}

		return Path;
	}

	// Exe dir, must be called before Current Directory changed
	static TChar* BaseDir()
	{
		static TChar Dir[512] = TEXTS("");
		if (!Dir[0])
		{
			Platform::GetCurrentDirW(Dir, 512);

			String Temp(Dir);
			Temp.Replace(TEXTS("\\"), TEXTS("/"));
			Temp += TEXTS('/');
			PlatformChars::Strcpy(Dir, 512, *Temp);
		}
		return Dir;
	}

	// Return the filename with ext£¬without any path information
	static String GetFileName(const String& InPath, bool WithExtension)
	{
		std::function<bool(const TChar&)> IsSlashOrBackslash = [](TChar C) { return C == TEXTS('/') || C == TEXTS('\\'); };
		std::function<bool(const TChar&)> IsNotSlashOrBackslash = [](TChar C) { return C != TEXTS('/') && C != TEXTS('\\'); };

		int32 StartPos = InPath.FindBySelectorFromEnd(IsSlashOrBackslash) + 1;
		int32 EndPos = InPath.FindBySelectorFromEnd(IsNotSlashOrBackslash) + 1;

		String Name = (StartPos <= EndPos) ? InPath.Range(StartPos, EndPos) : InPath;

		if (!WithExtension)
		{
			int32 ExtPos = Name.FindFromEnd(TEXTS("."), Name.Len());
			if (ExtPos > 0)
			{
				Name = Name.Left(ExtPos);
			}
		}

		return Name;
	}

	// Returns the path information of the filename
	static String GetFileDir(const String& InPath)
	{
		std::function<bool(const TChar&)> IsSlashOrBackslash = [](TChar C) { return C == TEXTS('/') || C == TEXTS('\\'); };

		String Dir;
		int32 Pos = InPath.FindBySelectorFromEnd(IsSlashOrBackslash);
		if (Pos != -1)
		{
			Dir = InPath.Left(Pos);
		}
		else
		{
			Dir = InPath;
		}

		return Dir;
	}
};


