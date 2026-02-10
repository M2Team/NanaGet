using ImageMagick;
using Mile.Project.Helpers;
using System.Collections.Concurrent;

namespace NanaGet.ProjectAssetsGenerator
{
    internal class Program
    {
        static string RepositoryRoot = GitRepository.GetRootPath();

        static void Main(string[] args)
        {
            {
                string SourcePath = RepositoryRoot + @"\Assets\OriginalAssets\NanaGet";

                string OutputPath = RepositoryRoot + @"\Assets\PackageAssets";

                ConcurrentDictionary<int, MagickImage> StandardSources =
                    new ConcurrentDictionary<int, MagickImage>();
                ConcurrentDictionary<int, MagickImage> StandardIconSources =
                    new ConcurrentDictionary<int, MagickImage>();
                ConcurrentDictionary<int, MagickImage> ContrastBlackSources =
                    new ConcurrentDictionary<int, MagickImage>();
                ConcurrentDictionary<int, MagickImage> ContrastWhiteSources =
                    new ConcurrentDictionary<int, MagickImage>();

                ConcurrentDictionary<int, MagickImage> MetadataFileSources =
                    new ConcurrentDictionary<int, MagickImage>();

                foreach (var AssetSize in ProjectAssetsUtilities.AssetSizes)
                {
                    StandardSources[AssetSize] = new MagickImage(string.Format(
                        @"{0}\{1}\{1}_{2}.png",
                        SourcePath,
                        "Standard",
                        AssetSize));
                    StandardIconSources[AssetSize] = new MagickImage(string.Format(
                        @"{0}\{1}\{1}_{2}.png",
                        SourcePath.Replace("OriginalAssets", "OriginalAssetsOptimized"),
                        "Standard",
                        AssetSize));
                    ContrastBlackSources[AssetSize] = new MagickImage(string.Format(
                        @"{0}\{1}\{1}_{2}.png",
                        SourcePath,
                        "ContrastBlack",
                        AssetSize));
                    ContrastWhiteSources[AssetSize] = new MagickImage(string.Format(
                        @"{0}\{1}\{1}_{2}.png",
                        SourcePath,
                        "ContrastWhite",
                        AssetSize));

                    MetadataFileSources[AssetSize] = new MagickImage(string.Format(
                        @"{0}\{1}\{1}_{2}.png",
                        SourcePath,
                        "MetadataFile",
                        AssetSize));
                }

                ProjectAssetsUtilities.GeneratePackageApplicationImageAssets(
                    StandardSources,
                    ContrastBlackSources,
                    ContrastWhiteSources,
                    OutputPath);

                ProjectAssetsUtilities.GeneratePackageFileAssociationImageAssets(
                    MetadataFileSources,
                    OutputPath,
                    @"MetadataFile");

                ProjectAssetsUtilities.GenerateIconFile(
                    StandardIconSources,
                    OutputPath + @"\..\NanaGet.ico");

                StandardSources[64].Write(
                    OutputPath + @"\..\NanaGet.png");
            }

            {
                string SourcePath = RepositoryRoot + @"\Assets\OriginalAssets\NanaGetPreview";

                string OutputPath = RepositoryRoot + @"\Assets\PreviewPackageAssets";

                ConcurrentDictionary<int, MagickImage> StandardSources =
                    new ConcurrentDictionary<int, MagickImage>();
                ConcurrentDictionary<int, MagickImage> StandardIconSources =
                   new ConcurrentDictionary<int, MagickImage>();
                ConcurrentDictionary<int, MagickImage> ContrastBlackSources =
                    new ConcurrentDictionary<int, MagickImage>();
                ConcurrentDictionary<int, MagickImage> ContrastWhiteSources =
                    new ConcurrentDictionary<int, MagickImage>();

                ConcurrentDictionary<int, MagickImage> MetadataFileSources =
                    new ConcurrentDictionary<int, MagickImage>();

                foreach (var AssetSize in ProjectAssetsUtilities.AssetSizes)
                {
                    StandardSources[AssetSize] = new MagickImage(string.Format(
                        @"{0}\{1}\{1}_{2}.png",
                        SourcePath,
                        "Standard",
                        AssetSize));
                    StandardIconSources[AssetSize] = new MagickImage(string.Format(
                        @"{0}\{1}\{1}_{2}.png",
                        SourcePath.Replace("OriginalAssets", "OriginalAssetsOptimized"),
                        "Standard",
                        AssetSize));
                    ContrastBlackSources[AssetSize] = new MagickImage(string.Format(
                        @"{0}\{1}\{1}_{2}.png",
                        SourcePath,
                        "ContrastBlack",
                        AssetSize));
                    ContrastWhiteSources[AssetSize] = new MagickImage(string.Format(
                        @"{0}\{1}\{1}_{2}.png",
                        SourcePath,
                        "ContrastWhite",
                        AssetSize));

                    MetadataFileSources[AssetSize] = new MagickImage(string.Format(
                        @"{0}\{1}\{1}_{2}.png",
                        SourcePath,
                        "MetadataFile",
                        AssetSize));
                }

                ProjectAssetsUtilities.GeneratePackageApplicationImageAssets(
                    StandardSources,
                    ContrastBlackSources,
                    ContrastWhiteSources,
                    OutputPath);

                ProjectAssetsUtilities.GeneratePackageFileAssociationImageAssets(
                    MetadataFileSources,
                    OutputPath,
                    @"MetadataFile");

                ProjectAssetsUtilities.GenerateIconFile(
                    StandardIconSources,
                    OutputPath + @"\..\NanaGetPreview.ico");

                StandardSources[64].Write(
                    OutputPath + @"\..\NanaGetPreview.png");
            }

            Console.WriteLine(
                "NanaGet.ProjectAssetsGenerator task has been completed.");
            Console.ReadKey();
        }
    }
}
