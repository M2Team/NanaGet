using Mile.Project.Helpers;
using System.Text;
using System.Xml;

namespace NanaGet.RefreshPackageVersion
{
    internal class Program
    {
        static (int Major, int Minor) Version = (1, 2);
        static string BuildStartDay = "2022-04-25";

        static string GenerateVersionString()
        {
            return string.Format(
                "{0}.{1}.{2}.0",
                Version.Major,
                Version.Minor,
                DateTime.Today.Subtract(DateTime.Parse(BuildStartDay)).TotalDays);
        }

        static string RepositoryRoot = GitRepository.GetRootPath();

        static void RefreshAppxManifestVersion()
        {
            string FilePath = string.Format(
                @"{0}\NanaGetPackage\Package.appxmanifest",
                RepositoryRoot);

            XmlDocument Document = new XmlDocument();
            Document.PreserveWhitespace = true;
            Document.Load(FilePath);

            XmlNode? PackageNode = Document["Package"];
            if (PackageNode != null)
            {
                XmlNode? IdentityNode = PackageNode["Identity"];
                if (IdentityNode != null)
                {
                    XmlAttribute? VersionAttribute =
                        IdentityNode.Attributes["Version"];
                    if (VersionAttribute != null)
                    {
                        FileUtilities.SaveTextToFileAsUtf8Bom(
                            FilePath,
                            File.ReadAllText(FilePath).Replace(
                                VersionAttribute.Value,
                                GenerateVersionString()));
                    }
                }
            }
        }

        static void ReplaceFileContentViaStringList(
            string FilePath,
            List<string> FromList,
            List<string> ToList)
        {
            if (FromList.Count != ToList.Count)
            {
                throw new ArgumentOutOfRangeException();
            }

            string Content = File.ReadAllText(FilePath, Encoding.UTF8);

            for (int Index = 0; Index < FromList.Count; ++Index)
            {
                Content = Content.Replace(FromList[Index], ToList[Index]);
            }

            if (Path.GetExtension(FilePath) == ".rc")
            {
                File.WriteAllText(FilePath, Content, Encoding.Unicode);
            }
            else
            {
                FileUtilities.SaveTextToFileAsUtf8Bom(FilePath, Content);
            }
        }

        static bool SwitchToPreview = true;

        static List<string> ReleaseStringList = new List<string>
        {
            "DisplayName=\"NanaGet\"",
            "Name=\"40174MouriNaruto.NanaGet\"",
            "<DisplayName>NanaGet</DisplayName>",
            "<Content Include=\"..\\Assets\\PackageAssets\\**\\*\">",
            "NanaGet.ico",
        };

        static List<string> PreviewStringList = new List<string>
        {
            "DisplayName=\"NanaGet Preview\"",
            "Name=\"40174MouriNaruto.NanaGetPreview\"",
            "<DisplayName>NanaGet Preview</DisplayName>",
            "<Content Include=\"..\\Assets\\PreviewPackageAssets\\**\\*\">",
            "NanaGetPreview.ico",
        };

        static List<string> FileList = new List<string>
        {
            @"{0}\NanaGetPackage\Package.appxmanifest",
            @"{0}\NanaGetPackage\NanaGetPackage.wapproj",
            @"{0}\NanaGet\NanaGetResources.rc",
        };

        static void SwitchDevelopmentChannel()
        {
            foreach (var FilePath in FileList)
            {
                ReplaceFileContentViaStringList(
                    string.Format(FilePath, RepositoryRoot),
                    SwitchToPreview ? ReleaseStringList : PreviewStringList,
                    SwitchToPreview ? PreviewStringList : ReleaseStringList);
            }
        }

        static void Main(string[] args)
        {
            RefreshAppxManifestVersion();

            SwitchDevelopmentChannel();

            Console.WriteLine(
                "NanaGet.RefreshPackageVersion task has been completed.");
            Console.ReadKey();
        }
    }
}
